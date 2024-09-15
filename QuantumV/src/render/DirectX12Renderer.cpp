#include "DirectX12Renderer.h"

#ifdef QV_RENDERER_DX12

#include <DirectXMath.h>
#include "QuantumV/core/Log.h"

namespace QuantumV {
	void DirectX12Renderer::Init(void* window_handle, uint32_t width, uint32_t height) {
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