#include "CameraFactory.h"

namespace QuantumV {
	ICamera* CameraFactory::CreateCamera(Ref<IAllocator> allocator, glm::vec3 position, glm::vec3 focusPoint, float fov, float width, float height) {
		if (allocator->GetRenderAPI() == RenderAPI::D3D12) {
			return new D3D12::Camera(allocator, position, focusPoint, fov, width, height);
		}
		else {
			return nullptr; // Vulkan camera
		}
	}
}