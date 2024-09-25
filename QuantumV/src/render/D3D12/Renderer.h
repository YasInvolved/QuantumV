#ifdef QV_PLATFORM_WINDOWS

#pragma once

#include <QuantumV/core/Base.h>
#include "../IRenderer.h"
#include "../../core/Window.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Allocator.h"

namespace QuantumV::D3D12 {
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class Renderer : virtual public IRenderer {
	public:
		Renderer(const Window* window);

		void Initialize() override;
		void Draw() override;
	private:
		const Window* m_window;
		ComPtr<IDXGIFactory7> m_factory;
		ComPtr<IDXGIAdapter4> m_adapter;

		ComPtr<ID3D12Device10> m_device;
		ComPtr<ID3D12CommandQueue> m_graphicsQueue;

		std::shared_ptr<Allocator> m_allocator;
	};
}

#endif