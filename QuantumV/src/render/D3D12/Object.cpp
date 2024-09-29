#include "Object.h"
#include <QuantumV/core/Log.h>
#include <glm/gtc/matrix_transform.hpp>

namespace QuantumV::D3D12 {
	struct ConstantBuffer {
		glm::mat4 modelMatrix;
	};

	Object::Object(const std::string& filepath, Ref<IAllocator> allocator)
	{
		{ // load object data
			tinyobj::ObjReader reader;
			tinyobj::ObjReaderConfig config = {};
			config.mtl_search_path = "./";
			reader.ParseFromFile(filepath, config);
			if (!reader.Error().empty()) {
				QV_CORE_ERROR("Failed to load object {0}: {1}", filepath, reader.Error());
				return;
			}

			if (!reader.Warning().empty()) {
				QV_CORE_WARN(reader.Warning());
			}

			const tinyobj::attrib_t& attrib = reader.GetAttrib();
			const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
			const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

			for (size_t s = 0; s < shapes.size(); s++) {
				const tinyobj::mesh_t& mesh = shapes[s].mesh;

				size_t index_offset = 0;
				for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
					int fv = mesh.num_face_vertices[f];

					for (size_t v = 0; v < fv; v++) {
						tinyobj::index_t idx = mesh.indices[index_offset + v];
						tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
						tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
						tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

						m_vertices.push_back(Vertex{ {vx, vy, vz}, {1.0f, 0.0f, 0.0f, 1.0f} });
						m_indices.push_back(index_offset + v);

						// TODO: normals and texcoords
					}

					index_offset += fv;
				}
			}
		} // load object data

		// allocate buffers
		const size_t cbSize = (sizeof(ConstantBuffer) + 255) & ~255;
		m_vbHandle = allocator->AllocateVertexBuffer(sizeof(Vertex) * m_vertices.size());
		m_ibHandle = allocator->AllocateIndexBuffer(sizeof(uint32_t) * m_indices.size());
		m_cbHandle = allocator->AllocateBuffer(cbSize);

		void* bufferData;
		m_vbHandle.GetD3D12Resource()->Map(0, nullptr, &bufferData);
		assert(bufferData != nullptr);
		memcpy(bufferData, m_vertices.data(), sizeof(Vertex) * m_vertices.size());
		m_vbHandle.GetD3D12Resource()->Unmap(0, nullptr);

		m_ibHandle.GetD3D12Resource()->Map(0, nullptr, &bufferData);
		assert(bufferData != nullptr);
		memcpy(bufferData, m_indices.data(), sizeof(uint32_t) * m_indices.size());
		m_ibHandle.GetD3D12Resource()->Unmap(0, nullptr);
		
		UpdateConstantBuffer();
	}

	VertexBufferHandle Object::GetVertexBuffer() {
		return m_vbHandle;
	}

	IndexBufferHandle Object::GetIndexBuffer() {
		return m_ibHandle;
	}

	BufferHandle Object::GetConstantBuffer() {
		return m_cbHandle;
	}

	void Object::SetPositon(float x, float y, float z) {
		m_translation = glm::vec3(x, y, z);
		UpdateConstantBuffer();
	}

	void Object::SetScale(float x, float y, float z) {
		m_translation = glm::vec3(x, y, z);
		UpdateConstantBuffer();
	}

	void Object::UpdateConstantBuffer() {
		void* bufferData;
		// prepare model matrix
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_translation);
		model = glm::scale(model, m_scale);
		model *= m_rotation;

		ConstantBuffer buffer = { glm::transpose(model) };

		m_cbHandle.GetD3D12Resource()->Map(0, nullptr, &bufferData);
		assert(bufferData != nullptr);
		memcpy(bufferData, &buffer, sizeof(ConstantBuffer));
		m_cbHandle.GetD3D12Resource()->Unmap(0, nullptr);
	}

	void Object::Move(float delta_x, float delta_y, float delta_z) {
		m_translation += glm::vec3(delta_x, delta_y, delta_z);
		UpdateConstantBuffer();
	}

	void Object::Scale(float delta_x, float delta_y, float delta_z) {
		m_scale += glm::vec3(delta_x, delta_y, delta_z);
		UpdateConstantBuffer();
	}

	void Object::Rotate(float angle_x, float angle_y, float angle_z) {
		m_rotation = glm::rotate(m_rotation, angle_x, { 1.0f, 0.0f, 0.0f });
		m_rotation = glm::rotate(m_rotation, angle_y, { 0.0f, 1.0f, 0.0f });
		m_rotation = glm::rotate(m_rotation, angle_z, { 0.0f, 0.0f, 1.0f });
		UpdateConstantBuffer();
	}
}