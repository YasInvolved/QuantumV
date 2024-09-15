#pragma once

#ifdef QV_RENDERER_DX12

#include "IRenderer.h"
#include <Windows.h>s
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

// for simplicity
using namespace Microsoft::WRL;

namespace QuantumV {
	class DirectX12Renderer : public IRenderer {
	public:
		void Init(void* window_handle, uint32_t width, uint32_t height) override;
		void Clear(float r, float g, float b, float a) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void BindPipeline(Pipeline* pipeline) override;
		void Draw(int vertex_count, int start_index = 0) override;

		//Shader* CreateShader(const std::string& shader_path) override;
		//Texture* CreateTexture(const std::string& texture_path) override;
		//VertexBuffer* CreateVertexBuffer(const void* data, size_t size) override;

		void Resize(uint32_t new_width, uint32_t new_height) override;

	private:
		ComPtr<IDXGIFactory7> factory;
		ComPtr<ID3D12Device> device;
		ComPtr<ID3D12CommandQueue> commandQueue;

		void InitializePipeline();
	};
}

#endif