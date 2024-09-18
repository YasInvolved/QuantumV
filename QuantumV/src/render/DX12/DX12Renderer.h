#pragma once

#ifdef QV_RENDERER_DX12

#include "../IRenderer.h"
#include "../../util/Timer.h"
#include <Windows.h>s
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

// for simplicity
using namespace Microsoft::WRL;

namespace QuantumV {
	class DX12Renderer : public IRenderer {
	public:
		void Init(void* window_handle, uint32_t width, uint32_t height) override;
		void Clear(float r, float g, float b, float a) override;
		//void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		//void BindPipeline(Pipeline* pipeline) override;
		void Draw(int vertex_count, int start_index = 0) override;

		//Shader* CreateShader(const std::string& shader_path) override;
		//Texture* CreateTexture(const std::string& texture_path) override;
		//VertexBuffer* CreateVertexBuffer(const void* data, size_t size) override;

		void Resize(uint32_t new_width, uint32_t new_height) override;

	private:
		uint32_t m_width, m_height;
		HWND m_hwnd;
		const uint32_t m_frameCount = 3; // triple buffering
		uint32_t m_frameIndex;

		ComPtr<IDXGIFactory7> m_factory;
		ComPtr<ID3D12Device10> m_device;
		ComPtr<ID3D12CommandQueue> m_queue;
		ComPtr<ID3D12CommandAllocator> m_commandAllocatorDirect;

		ComPtr<IDXGISwapChain4> m_swapchain;

		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		std::vector<ComPtr<ID3D12Resource>> m_renderTargets;

		void WaitForPreviousFrame();
	};
}

#endif