#include "RendererFactory.h"
#include "D3D12/Renderer.h"

namespace QuantumV {
	Scope<IRenderer> RendererFactory::CreateRenderer(const Window* window, RenderAPI renderapi) {
		if (renderapi == RenderAPI::D3D12) {
			return std::make_unique<D3D12::Renderer>(window);
		}
		return nullptr;
	}
}