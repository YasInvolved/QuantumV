#include "DX12Renderer.h"

#ifdef QV_RENDERER_DX12

#include <future>
#include <array>
#include <dxgidebug.h>
#include "d3dx12.h"
#include "QuantumV/core/Log.h"

namespace QuantumV {
	void DX12Renderer::Init(void* window_handle, uint32_t width, uint32_t height) {
		#ifdef QV_DEBUG
		ComPtr<ID3D12Debug1> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(TRUE);
			debugController->SetEnableSynchronizedCommandQueueValidation(TRUE);
		}
		
		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		}
		#endif

		m_hwnd = reinterpret_cast<HWND>(window_handle);
		m_width = width;
		m_height = height;

		HRESULT result = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory));

		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to initialize DX12: {}", result);
			return;
		}

		ComPtr<IDXGIAdapter> adapter;
		result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		if (FAILED(result)) {
			QV_CORE_WARN("Failed to find WARP capable device");
			m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
		}

		result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		if (FAILED(result)) {
			QV_CORE_ERROR("Failed to create D3D12 Device");
			return;
		}

		{ // no need to store queue desc further let it get destroyed
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			result = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_queue));
			if (FAILED(result)) {
				QV_CORE_ERROR("Failed to create D3D12 Device command queue");
				return;
			}
		}

		// we dont need the temp swapchain and it's desc so that's why theres scope here
		{
			DXGI_SWAP_CHAIN_DESC swapchainDesc = {};
			swapchainDesc.BufferCount = m_frameCount;
			swapchainDesc.BufferDesc.Width = m_width;
			swapchainDesc.BufferDesc.Height = m_height;
			swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapchainDesc.OutputWindow = m_hwnd;
			swapchainDesc.SampleDesc.Count = 1;
			swapchainDesc.Windowed = TRUE;

			ComPtr<IDXGISwapChain> swapchain; // i meant this one
			result = m_factory->CreateSwapChain(m_queue.Get(), &swapchainDesc, &swapchain);
			if (FAILED(result)) {
				QV_CORE_ERROR("Failed to create D3D12 Swapchain: {}", result);
				return;
			}
			swapchain.As<IDXGISwapChain4>(&m_swapchain);
		}

		m_factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);
		m_frameIndex = m_swapchain->GetCurrentBackBufferIndex();

		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = m_frameCount;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

			result = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
			if (FAILED(result)) {
				QV_CORE_ERROR("Failed to create render target heap descriptor");
				return;
			}
		}

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
			m_renderTargets.resize(m_frameCount);
			for (uint32_t i = 0; i < m_frameCount; i++) {
				m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
				m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
			}
		}

		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocatorDirect));
	}

	void DX12Renderer::Clear(float r, float g, float b, float a) {
	}

	void DX12Renderer::WaitForPreviousFrame() {
	}

	//void DirectX12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}
	//void DirectX12Renderer::BindPipeline(Pipeline* pipeline) {}
	void DX12Renderer::Draw(int vertex_count, int start_index) {}

	//Shader* DirectX12Renderer::CreateShader(const std::string& shader_path) { return {}; }
	//Texture* DirectX12Renderer::CreateTexture(const std::string& texture_path) { return {}; }
	//VertexBuffer* DirectX12Renderer::CreateVertexBuffer(const void* data, size_t size) { return {}; }

	void DX12Renderer::Resize(uint32_t new_width, uint32_t new_height) {}
}

#endif