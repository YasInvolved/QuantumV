#pragma once

#include "../IRenderer.h"
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <stdint.h>
#include <vector>
#include <array>
#include <D3D12MemAlloc.h>

namespace QuantumV {
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class DX12Renderer : public IRenderer {
	public:
		DX12Renderer(Window* window);
		~DX12Renderer();

		void Init() override;
		void RenderFrame() override;
		void Shutdown() override;
		void Resize(uint32_t new_width, uint32_t new_height) override;
		RenderAPI GetRenderAPI() const override { return RenderAPI::DX12; }

		D3D12MA::Allocator* GetAllocator() { return m_allocator; }
	private:
		Window* m_window;

		// device and factory
		ComPtr<IDXGIFactory7> m_factory;
		ComPtr<IDXGIAdapter4> m_adapter;
		ComPtr<ID3D12Device10> m_device;
		
		// swapchain
		const uint32_t m_frameCount = 3;
		uint32_t m_currentFrame = 0;
		ComPtr<IDXGISwapChain4> m_swapchain;
		uint32_t m_rtvDescriptorSize;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		std::vector<ComPtr<ID3D12Resource>> m_renderTargets;

		// command lists
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_graphicsCommandList;

		// pipeline
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3DBlob> m_vertexShader;
		ComPtr<ID3DBlob> m_pixelShader;
		ComPtr<ID3D12PipelineState> m_graphicsPipelineState;

		// queues
		ComPtr<ID3D12CommandQueue> m_graphicsQueue;

		// synchronization
		uint64_t m_fenceValue = 0;
		ComPtr<ID3D12Fence> m_renderFence;
		HANDLE m_renderEvent;

		// memory
		D3D12MA::Allocator* m_allocator;
	};
}