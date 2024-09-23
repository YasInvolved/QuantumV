#include "IObject.h"
#include "DX12/DX12Object.h"

namespace QuantumV {
	IObject* IObject::CreateObject(IRenderer* renderer, const std::string& path) {
		if (renderer->GetRenderAPI() == RenderAPI::DX12) {
			return new DX12Object(static_cast<DX12Renderer*>(renderer), path);
		}
		else {
			// return new VulkanObject();
		}
	}
}