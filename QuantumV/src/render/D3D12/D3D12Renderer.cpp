#include "../../core/Window.h"
#include <QuantumV/render/D3D12/D3D12Renderer.h>

namespace QuantumV::D3D12 {
	Renderer::Renderer(Window& window) : BaseRenderer(window) {

	}

	Renderer::~Renderer() {
		Shutdown();
	}

	void Renderer::Init() {

	}

	void Renderer::Render() {

	}

	void Renderer::Shutdown() {

	}
}