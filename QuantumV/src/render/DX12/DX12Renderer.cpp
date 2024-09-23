#include "DX12Renderer.h"
#include <QuantumV/core/Log.h>
#include <d3dcompiler.h>
#include <future>
#include "../Primitives.h"

namespace QuantumV {
	DX12Renderer::DX12Renderer(Window* window)
		: m_window(window) {}

	DX12Renderer::~DX12Renderer() {
		Shutdown();
	}

	void DX12Renderer::Init() {
		HRESULT result;
		QV_CORE_TRACE("Rendering API: DX12");
		{
			ComPtr<IDXGIFactory> tempFactory;
			result = CreateDXGIFactory(IID_PPV_ARGS(&tempFactory));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create DXGI Factory. Result: {}", result);
				return;
			}

			result = tempFactory.As(&m_factory);
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to acquire DXGI Factory V7. Result {}", result);
			}
		}

		{
			ComPtr<IDXGIAdapter> tempAdapter;
			result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&tempAdapter));
			if (FAILED(result)) {
				QV_CORE_WARN("Failed to acquire WARP-compatible adapter. Falling back to regular");
				result = m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tempAdapter));
				if (FAILED(result)) {
					QV_CORE_CRITICAL("Failed to acquire DX12-Compatible adapter.");
					return;
				}
			}

			result = tempAdapter.As(&m_adapter);
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Adapter unknown error.");
				return;
			}
		}

		result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to create D3D12 Device");
			return;
		}

		{
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

			m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_graphicsQueue));
		}

		{
			DXGI_SWAP_CHAIN_DESC1 desc = {};
			desc.BufferCount = m_frameCount;
			desc.Width = m_window->getWidth();
			desc.Height = m_window->getHeight();
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

			ComPtr<IDXGISwapChain1> tempSwapchain;
			result = m_factory->CreateSwapChainForHwnd(
				m_graphicsQueue.Get(),
				m_window->getHWND(),
				&desc,
				nullptr,
				nullptr,
				&tempSwapchain
			);

			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create swapchain");
				return;
			}

			result = tempSwapchain.As(&m_swapchain);
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create swapchain v4");
				return;
			}

			m_factory->MakeWindowAssociation(m_window->getHWND(), DXGI_MWA_NO_ALT_ENTER);
		}
		
		m_currentFrame = m_swapchain->GetCurrentBackBufferIndex();

		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.NumDescriptors = m_frameCount;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			
			m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
			m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
			m_renderTargets.resize(m_frameCount);
			for (uint32_t i = 0; i < m_frameCount; i++) {
				m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
				m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
				rtvHandle.ptr += m_rtvDescriptorSize;
			}
		}

		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_graphicsCommandList));
		m_graphicsCommandList->Close();

		{
			std::array<D3D12_ROOT_PARAMETER, 1> rootParameters = {};
			rootParameters[0].Descriptor.RegisterSpace = 0;
			rootParameters[0].Descriptor.ShaderRegister = 0;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

			D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
			rootSigDesc.NumParameters = rootParameters.size();
			rootSigDesc.pParameters = rootParameters.data();
			rootSigDesc.NumStaticSamplers = 0;
			rootSigDesc.pStaticSamplers = nullptr;

			ComPtr<ID3DBlob> rootSignature;
			ComPtr<ID3DBlob> error;
			result = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignature, &error);
			if (FAILED(result)) {
				if (error->GetBufferPointer() != nullptr) {
					QV_CORE_CRITICAL("Failed to serialize root signature: {}", (char*)error->GetBufferPointer());
					return;
				}
				else {
					QV_CORE_CRITICAL("Failed to serialize root signature");
				}
			}

			result = m_device->CreateRootSignature(0, rootSignature->GetBufferPointer(), rootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create root signature");
				return;
			}
		}
		
		auto vertexShaderFuture = std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/dx12/VertexShader.cso", &m_vertexShader);
		auto pixelShaderFuture = std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/dx12/PixelShader.cso", &m_pixelShader);

		constexpr auto inputElementDescs = Vertex::GetElementDescDX12();

		vertexShaderFuture.wait();
		pixelShaderFuture.wait();

		{
			D3D12_RASTERIZER_DESC rasterizerDesc = {};
			rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
			rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			rasterizerDesc.ForcedSampleCount = 0;
			rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

			D3D12_BLEND_DESC blendDesc = {};
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			
			D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
			rtBlendDesc.BlendEnable = FALSE;
			rtBlendDesc.LogicOpEnable = FALSE;
			rtBlendDesc.SrcBlend = D3D12_BLEND_ONE;
			rtBlendDesc.DestBlend = D3D12_BLEND_ZERO;
			rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
			rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
			rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			blendDesc.RenderTarget[0] = rtBlendDesc;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs.data(), inputElementDescs.size() };
			psoDesc.pRootSignature = m_rootSignature.Get();
			psoDesc.VS = { m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize() };
			psoDesc.PS = { m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize() };
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.NumRenderTargets = 1;
			psoDesc.SampleDesc.Count = 1;
			psoDesc.SampleDesc.Quality = 0;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.RasterizerState = rasterizerDesc;
			psoDesc.BlendState = blendDesc;
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;

			m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_graphicsPipelineState));
		}

		m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_renderFence));
		m_renderEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		{
			D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
			allocatorDesc.pAdapter = m_adapter.Get();
			allocatorDesc.pDevice = m_device.Get();
			
			D3D12MA::CreateAllocator(&allocatorDesc, &m_allocator);
		}
	}

	void DX12Renderer::RenderFrame() {

	}

	void DX12Renderer::Shutdown() {

	}

	void DX12Renderer::Resize(uint32_t new_width, uint32_t new_height) {

	}
}