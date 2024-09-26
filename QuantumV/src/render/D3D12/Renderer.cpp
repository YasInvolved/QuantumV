#include "Renderer.h"
#include <QuantumV/core/Log.h>
#include <future>
#include <d3dcompiler.h>
#ifdef QV_DEBUG
#include <cassert>
#else
#define assert(expression) UNREFERENCED_PARAMETER(expression)
#endif
namespace QuantumV::D3D12 {
	Renderer::Renderer(const Window* window)
		: m_window(window) 
	{

	}

	void Renderer::Initialize() {
		HRESULT result;
		result = CreateDXGIFactory(IID_PPV_ARGS(&m_factory));

		// debug layer
		#ifdef QV_DEBUG
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
				debugController->EnableDebugLayer();
			}
		}
		#endif

		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to create DXGI Factory");
			return;
		}

		// adapter
		result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter));
		if (FAILED(result)) {
			QV_CORE_WARN("Failed to acquire WARP-compatible adapter. Rolling back to default");
			result = m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_factory));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to acquire suitable adapter");
				return;
			}
		}

		std::string factoryDebugName;
		m_factory->SetPrivateData(WKPDID_D3DDebugObjectName, factoryDebugName.size(), factoryDebugName.data());

		// device
		result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to create D3D12 Device");
			return;
		}

		{ // queues
			D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
			graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			graphicsQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

			result = m_device->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(&m_graphicsQueue));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create device graphics queue");
				return;
			}
		}

		// allocator
		m_allocator = std::make_shared<Allocator>(m_adapter.Get(), m_device.Get());

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
				QV_CORE_CRITICAL("Failed to create DXGI Swapchain");
				return;
			}

			result = tempSwapchain.As(&m_swapchain);
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create DXGI Swapchain");
				return;
			}
		}

		m_currentFrame = m_swapchain->GetCurrentBackBufferIndex();

		{ // RTVs
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDesc.NumDescriptors = m_frameCount;
			
			m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap));
			m_rtvDescSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			assert((m_rtvDescSize > 0) && "RTV size is 0");

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
			m_renderTargets.resize(m_frameCount);
			for (size_t i = 0; i < m_frameCount; i++) {
				m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
				m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
				rtvHandle.ptr += m_rtvDescSize;
			}
		}

		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_graphicsCommandAllocator));
		m_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_graphicsCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(&m_graphicsCommandList)
		);
		m_graphicsCommandList->Close();

		{
			D3D12_DESCRIPTOR_RANGE cbvRange = {};
			cbvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			cbvRange.BaseShaderRegister = 0;
			cbvRange.NumDescriptors = 1;
			cbvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			cbvRange.RegisterSpace = 0;

			std::array<D3D12_ROOT_PARAMETER, 1> parameters = {};
			parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameters[0].DescriptorTable.NumDescriptorRanges = 1;
			parameters[0].DescriptorTable.pDescriptorRanges = &cbvRange;
			parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

			D3D12_ROOT_SIGNATURE_DESC desc = {};
			desc.NumParameters = parameters.size();
			desc.pParameters = parameters.data();
			desc.NumStaticSamplers = 0;
			desc.pStaticSamplers = nullptr;
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			ComPtr<ID3DBlob> rootSignature;
			ComPtr<ID3DBlob> error;
			result = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignature, &error);
			if (FAILED(result)) {
				QV_CORE_CRITICAL(
					"Failed to serialize root signature: {}", 
					error->GetBufferPointer() == nullptr ? "unknown error" : (char*)error->GetBufferPointer()
				);
				return;
			}

			result = m_device->CreateRootSignature(
				0,
				rootSignature->GetBufferPointer(),
				rootSignature->GetBufferSize(),
				IID_PPV_ARGS(&m_rootSignature)
			);

			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create root signature");
				return;
			}
		}

		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;
			auto vertexShaderSourceFuture = std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/d3d12/VertexShader.cso", &vertexShader);
			auto pixelShaderSourceFuture = std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/d3d12/PixelShader.cso", &pixelShader);

			constexpr auto inputElementDescs = Vertex::GetElementDescDX12();
			
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

			vertexShaderSourceFuture.wait();
			result = vertexShaderSourceFuture.get();
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to read vertex shader source code. Probably file doesn't exists");
				return;
			}
			psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
			
			pixelShaderSourceFuture.wait();
			result = pixelShaderSourceFuture.get();
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to read pixel shader source code. Probably file doesn't exists");
				return;
			}
			psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };


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

			result = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_graphicsPipeline));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create graphics pipeline");
				return;
			}
		}

		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_renderFence));
		m_renderEvent = CreateEvent(nullptr, FALSE, FALSE, "RenderEvent");
	}

	void Renderer::Draw() {

	}
}