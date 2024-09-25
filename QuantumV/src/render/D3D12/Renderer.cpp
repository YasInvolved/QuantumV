#include "Renderer.h"
#include <QuantumV/core/Log.h>

namespace QuantumV::D3D12 {
	Renderer::Renderer(const Window* window)
		: m_window(window) 
	{

	}

	void Renderer::Initialize() {
		HRESULT result;
		result = CreateDXGIFactory(IID_PPV_ARGS(&m_factory));

		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to create DXGI Factory");
			return;
		}

		// adapter
		result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter));
		if (FAILED(result)) {
			QV_CORE_WARN("Failed to acquire WARP-compatible adapter. Rolling back to default");
			result = m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_factory));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to acquire suitable adapter");
				return;
			}
		}

		// device
		result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		if (FAILED(result)) {
			QV_CORE_CRITICAL("Failed to create D3D12 Device");
			return;
		}

		{ // queues
			D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
			graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			graphicsQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

			result = m_device->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(&m_graphicsQueue));
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create device graphics queue");
				return;
			}
		}

		// allocator
		m_allocator = std::make_shared<Allocator>(m_adapter.Get(), m_device.Get());
	}

	void Renderer::Draw() {

	}
}