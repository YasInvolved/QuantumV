#pragma once

#include "../IObject.h"
#include <vector>

namespace QuantumV::D3D12 {
	class Object : public IObject {
	public:
		Object(const std::string& filepath, const Ref<IAllocator> allocator, std::optional<const std::string> materialPath);

		VertexBufferHandle GetVertexBuffer() override;
		IndexBufferHandle GetIndexBuffer() override;
		BufferHandle GetConstantBuffer() override;
		void SetPositon(float x, float y, float z) override;
		void SetScale(float x, float y, float z) override;
		void Move(float delta_x, float delta_y, float delta_z) override;
		void Scale(float delta_x, float delta_y, float delta_z) override;
		void Rotate(float angle_x, float angle_y, float angle_z) override;
	private:
		VertexBufferHandle m_vbHandle;
		IndexBufferHandle m_ibHandle;
		BufferHandle m_cbHandle;

		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		glm::vec3 m_translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };
		glm::mat4 m_rotation = glm::mat4(1.0f);

		void UpdateConstantBuffer();
	};
}