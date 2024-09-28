#pragma once

#include "../IObject.h"
#include <vector>
#include <tiny_obj_loader.h>
#include <mutex>

namespace QuantumV::D3D12 {
	class Object : public IObject {
	public:
		Object(const std::string& filepath, IAllocator* allocator);

		VertexBufferHandle GetVertexBuffer() override;
		IndexBufferHandle GetIndexBuffer() override;
		BufferHandle GetConstantBuffer() override;
		void SetPositon(float x, float y, float z) override;
		void SetScale(float x, float y, float z) override;
	private:
		VertexBufferHandle m_vbHandle;
		IndexBufferHandle m_ibHandle;
		BufferHandle m_cbHandle;

		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		glm::vec3 m_translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_rotation = { 0.0f, 0.0f, 0.0f };

		void UpdateConstantBuffer();
	};
}