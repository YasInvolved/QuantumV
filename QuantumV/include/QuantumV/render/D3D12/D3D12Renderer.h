#ifdef QV_PLATFORM_WINDOWS // guard in case the build target was linux or mac

#pragma once
#include <QuantumV/render/BaseRenderer.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace QuantumV::D3D12 {
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class QV_API Renderer : public BaseRenderer {
	public:
		Renderer(Window& window);
		~Renderer() override;

		void Init() override;
		void Render() override;
		void Shutdown() override;
	private:
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_graphicsQueue;
		ComPtr<IDXGIFactory7> m_factory;
		ComPtr<IDXGIAdapter4> m_adapter;
	};
}

#endif