#pragma once

#ifdef QV_RENDERER_DX12

#include "IRenderer.h"
#include <Windows.h>s
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl/client.h>
#include <D3D12MemAlloc.h>

// for simplicity
using namespace Microsoft::WRL;

namespace QuantumV {
	class DirectX12Renderer : public IRenderer {
	public:
		~DirectX12Renderer();

		void Init(const Window* window, uint32_t width, uint32_t height) override;
		void Clear(float r, float g, float b, float a) override;
		//void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		//void BindPipeline(Pipeline* pipeline) override;void Update()
		void Draw(int vertex_count, int start_index = 0) override;

		//Shader* CreateShader(const std::string& shader_path) override;
		//Texture* CreateTexture(const std::string& texture_path) override;
		//VertexBuffer* CreateVertexBuffer(const void* data, size_t size) override;

		void Resize(uint32_t new_width, uint32_t new_height) override;

	private:
		uint32_t m_width, m_height;
		uint32_t m_currentFrameIndex = 0;
		uint32_t m_rtvDescriptorSize;
		const uint32_t m_frameCount = 3;
		HWND m_hwnd;

		ComPtr<IDXGIFactory7> m_factory;
		ComPtr<IDXGIAdapter4> m_adapter;
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain4> m_swapchain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;

		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3DBlob> m_vertexShader;
		ComPtr<ID3DBlob> m_pixelShader;
		ComPtr<ID3D12PipelineState> m_pipelineState;

		D3D12MA::Allocator* m_allocator = nullptr;

		D3D12MA::Allocation* m_vertexBufferAllocation;
		ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		D3D12MA::Allocation* m_indexBufferAllocation;
		ComPtr<ID3D12Resource> m_indexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

		uint32_t m_cbSize;
		D3D12MA::Allocation* m_constantBufferAllocation;
		ComPtr<ID3D12Resource> m_constantBuffer;
		ComPtr<ID3D12DescriptorHeap> m_constantBufferDescriptorHeap;
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_constantBufferView;

		ComPtr<ID3D12DescriptorHeap> m_imguiDescriptorHeap;

		// camera variables
		float m_eyePosition[3] = { 0.0f, 0.0f, -5.0f };
		float m_focusPoint[3] = { 0.0f, 0.0f, 0.0f };
		float m_upDirection[3] = { 0.0f, 1.0f, 0.0f };

		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fenceEvent;
		uint64_t m_fenceValue = 0;

		void Update();
		void WaitForPreviousFrame();
	};
}

#endif