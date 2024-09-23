#include "IRenderer.h"

#include "DX12/DX12Renderer.h"

namespace QuantumV {
	IRenderer* IRenderer::CreateRenderer(RenderAPI api, Window* window) {
		if (api == RenderAPI::DX12) {
			return new DX12Renderer(window);
		}
		else {
			// return new VulkanRenderer();
		}

		return nullptr;
	}
}