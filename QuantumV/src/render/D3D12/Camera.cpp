#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

struct CameraBuffer {
	glm::mat4 projMatrix;
	glm::mat4 viewMatrix;
};

namespace QuantumV::D3D12 {
	Camera::Camera(IAllocator* allocator, glm::vec3 position, glm::vec3 focusPoint, float fov, float width, float height)
		: m_position(position), m_focusPoint(focusPoint), m_fov(fov), m_width(width), m_height(height)
	{
		const size_t bufferSize = (sizeof(CameraBuffer) + 255) & ~255;
		m_cameraBuffer = allocator->AllocateBuffer(bufferSize);

		CameraBuffer buffer = {};

		glm::mat4 viewMatrix = glm::lookAtLH(m_position, m_focusPoint, {0.0f, 1.0f, 0.0f});
		glm::mat4 projMatrix = glm::perspectiveFovLH(m_fov, m_width, m_height, 0.1f, 100.0f);

		buffer.projMatrix = glm::transpose(projMatrix);
		buffer.viewMatrix = glm::transpose(viewMatrix);

		void* bufferPtr;
		m_cameraBuffer.GetD3D12Resource()->Map(0, nullptr, &bufferPtr);
		assert(bufferPtr != nullptr);
		memcpy(bufferPtr, &buffer, sizeof(CameraBuffer));
		m_cameraBuffer.GetD3D12Resource()->Unmap(0, nullptr);
	}

	BufferHandle Camera::GetCameraBuffer() {
		return m_cameraBuffer;
	}
}