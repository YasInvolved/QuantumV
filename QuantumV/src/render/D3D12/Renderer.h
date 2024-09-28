#ifdef QV_PLATFORM_WINDOWS

#pragma once

#include <QuantumV/core/Base.h>
#include "../IRenderer.h"
#include "../IObject.h"
#include "../ICamera.h"
#include "../../core/Window.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Allocator.h"
#include <vector>
#include <array>

namespace QuantumV::D3D12 {
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class Renderer : public IRenderer {
	public:
		Renderer(const Window* window);

		void Initialize() override;
		void Draw() override;
		IAllocator* GetAllocator() override { return m_allocator.get(); }
		void AddObject(IObject* object) override { m_objects.push_back(object); }
		void AddCamera(ICamera* camera) override { m_camera = camera; }
	private:
		const Window* m_window;
		ComPtr<IDXGIFactory7> m_factory;
		ComPtr<IDXGIAdapter4> m_adapter;

		ComPtr<ID3D12Device10> m_device;
		ComPtr<ID3D12CommandQueue> m_graphicsQueue;
		ComPtr<ID3D12RootSignature> m_rootSignature;

		// memory
		Scope<Allocator> m_allocator;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		size_t m_rtvDescSize = 0;

		// swapchain
		const uint32_t m_frameCount = 3;
		uint32_t m_currentFrame = 0;
		ComPtr<IDXGISwapChain4> m_swapchain;
		std::vector<ComPtr<ID3D12Resource>> m_renderTargets;

		// commands
		ComPtr<ID3D12CommandAllocator> m_graphicsCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_graphicsCommandList;

		// pipelines
		ComPtr<ID3D12PipelineState> m_graphicsPipeline;

		// synchronization
		uint64_t m_renderFenceValue = 0;
		ComPtr<ID3D12Fence> m_renderFence;
		HANDLE m_renderEvent;

		// objects
		std::vector<IObject*> m_objects;

		// camera
		ICamera* m_camera;

		// viewport
		D3D12_VIEWPORT m_viewport = {};
		D3D12_RECT m_scissor = {};

		void Transition(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES prev, D3D12_RESOURCE_STATES after);
		void WaitForPreviousFrame();
	};
}

#endif