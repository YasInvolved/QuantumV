#ifdef QV_PLATFORM_WINDOWS

#include "../../core/Window.h"
#include <QuantumV/render/D3D12/D3D12Renderer.h>
#include <QuantumV/core/Log.h>

#ifdef QV_DEBUG
	#include <d3d12sdklayers.h>
	#include <dxgidebug.h>
#endif

namespace QuantumV::D3D12 {
	static ComPtr<ID3D12Debug1> s_d3d12DebugInterface;
	static ComPtr<IDXGIDebug> s_dxgiDebugInterface;

	Renderer::Renderer(Window& window) : BaseRenderer(window) {

	}

	Renderer::~Renderer() {
		Shutdown();
	}

	void Renderer::Init() {
		HRESULT result; // define variable for results
		QV_CORE_TRACE("Intitializing D3D12 Renderer");

		// enable debug layers if running in debug mode
		#ifdef QV_DEBUG
		{
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&s_d3d12DebugInterface)))) {
				s_d3d12DebugInterface->EnableDebugLayer();
			}

			DXGIGetDebugInterface1(0, IID_PPV_ARGS(&s_dxgiDebugInterface));
		}
		#endif

		// create factory
		QV_CORE_TRACE("Creating DXGI Factory");
		result = CreateDXGIFactory(IID_PPV_ARGS(&m_factory));
		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to cerate DXGI factory. Result code: {:d}", result);
			return;
		}

		// choose the best available adapter
		QV_CORE_TRACE("Selecting GPU");
		result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter));
		if (FAILED(result)) {
			QV_CORE_WARN("Failed to find WARP compatible GPU. Falling back to normal.");
			result = m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to find compatible GPU. Aborting.");
				return;
			}
		}

		// create device from chosen adapter
		QV_CORE_TRACE("Creating D3D12 Device");
		result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to create D3D12 Device. Aborting.");
			return;
		}

		{ // create graphics queue
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			
			result = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsQueue));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to craete graphics queue");
				return;
			}
		}
	}

	void Renderer::Render() {

	}

	void Renderer::Shutdown() {
		if (s_dxgiDebugInterface) {
			s_dxgiDebugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		}
	}
}

#endif