#include "DirectX12Renderer.h"

#ifdef QV_RENDERER_DX12

#include <DirectXMath.h>
#include "QuantumV/core/Log.h"

namespace QuantumV {
	void DirectX12Renderer::Init(void* window_handle, uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
		hwnd = reinterpret_cast<HWND>(window_handle);
		InitializePipeline();
	}

	void DirectX12Renderer::InitializePipeline() {
		#ifdef QV_DEBUG
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
				debugController->EnableDebugLayer();
			}
			else {
				QV_CORE_CRITICAL("Failed to initialize debug controller");
			}
		}
		#endif

		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&this->factory)))) {
			QV_CORE_CRITICAL("Failed to create DXGI Factory");
		}

		if (FAILED(D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&this->device)))) {
			QV_CORE_CRITICAL("Failed to create DX12 Device");
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		this->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&this->commandQueue));

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = 2;
		swapchainDesc.Width = this->width;
		swapchainDesc.Height = this->height;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo = FALSE;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain1> swapchainTemp;
		factory->CreateSwapChainForHwnd(this->commandQueue.Get(), this->hwnd, &swapchainDesc, nullptr, nullptr, &swapchainTemp);
		factory->MakeWindowAssociation(this->hwnd, DXGI_MWA_NO_ALT_ENTER);
		swapchainTemp.As(&this->swapchain);

		this->currentFrameIndex = this->swapchain->GetCurrentBackBufferIndex();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 2;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (uint32_t i = 0; i < 2; i++) {
			swapchain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += rtvDescriptorSize;
		}

		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->commandAllocator));

		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(&this->commandList));
		this->commandList->Close();

		device->CreateFence(this->fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence));
		this->fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	}

	void DirectX12Renderer::Clear(float r, float g, float b, float a) {}
	void DirectX12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}
	void DirectX12Renderer::BindPipeline(Pipeline* pipeline) {}
	void DirectX12Renderer::Draw(int vertex_count, int start_index) {}

	//Shader* DirectX12Renderer::CreateShader(const std::string& shader_path) { return {}; }
	//Texture* DirectX12Renderer::CreateTexture(const std::string& texture_path) { return {}; }
	//VertexBuffer* DirectX12Renderer::CreateVertexBuffer(const void* data, size_t size) { return {}; }

	void DirectX12Renderer::Resize(uint32_t new_width, uint32_t new_height) {}
}

#endif