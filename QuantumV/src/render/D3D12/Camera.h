#pragma once

#include "../ICamera.h"
#include "../IAllocator.h"
#include <QuantumV/core/Base.h>

namespace QuantumV::D3D12 {
	class Camera : public ICamera {
	public:
		Camera(Ref<IAllocator> allocator, glm::vec3 position, glm::vec3 focusPoint, float fov, float width, float height);

		BufferHandle GetCameraBuffer() override;
	private:
		BufferHandle m_cameraBuffer;
		glm::vec3 m_position;
		glm::vec3 m_focusPoint;
		float m_fov;
		float m_width, m_height;
	};
}