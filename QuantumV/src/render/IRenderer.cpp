#include "IRenderer.h"

namespace QuantumV {
	IRenderer* IRenderer::CreateRenderer(RenderAPI api) {
		if (api == RenderAPI::DX12) {
			// return new DX12Renderer();
		}
		else {
			// return new VulkanRenderer();
		}

		return nullptr;
	}
}