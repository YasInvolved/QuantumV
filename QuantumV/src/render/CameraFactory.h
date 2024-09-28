#pragma once

#include "D3D12/Camera.h"

namespace QuantumV {
	class CameraFactory {
	public:
		static ICamera* CreateCamera(IAllocator* allocator, glm::vec3 position, glm::vec3 focusPoint, float fov, float width, float height);
	};
}