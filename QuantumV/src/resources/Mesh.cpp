#include <QuantumV/resources/Mesh.h>
#include <QuantumV/core/Log.h>
#include <tiny_obj_loader.h>

namespace QuantumV {
	void Mesh::Load(const std::string& filePath) {
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig config = {};
		config.mtl_search_path = "./";

		reader.ParseFromFile(filePath, config);
		if (!reader.Error().empty()) {
			QV_CORE_ERROR("Failed to load mesh {0}: {1}", filePath, reader.Error());
			return;
		}

		if (!reader.Warning().empty()) {
			QV_CORE_WARN(reader.Warning());
		}

		const tinyobj::attrib_t& attrib = reader.GetAttrib();
		const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
		
		for (size_t s = 0; s < shapes.size(); s++) {
			const tinyobj::mesh_t& mesh = shapes[s].mesh;

			size_t index_offset = 0;
			for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
				int fv = mesh.num_face_vertices[f];

				bool hasNormals = true;
				for (size_t v = 0; v < fv; v++) {
					tinyobj::index_t idx = mesh.indices[index_offset + v];
					uint32_t index = index_offset + v;

					// positions
					glm::vec3 position = {};
					position.x = attrib.vertices[3 * idx.vertex_index + 0];
					position.y = attrib.vertices[3 * idx.vertex_index + 1];
					position.z = attrib.vertices[3 * idx.vertex_index + 2];

					// normals
					glm::vec3 normal = {};
					if (idx.normal_index >= 0) {
						normal.x = attrib.normals[3 * idx.normal_index + 0];
						normal.y = attrib.normals[3 * idx.normal_index + 1];
						normal.z = attrib.normals[3 * idx.normal_index + 2];
					}
					else {
						hasNormals = false;
					}

					// colors
					glm::vec4 color = {};
					color.r = attrib.colors[3 * idx.vertex_index + 0];
					color.g = attrib.colors[3 * idx.vertex_index + 1];
					color.b = attrib.colors[3 * idx.vertex_index + 2];
					color.a = 1.0f;

					m_vertices.emplace_back(position, normal, color);
					m_indices.emplace_back(index);
				}

				if (!hasNormals) {
					tinyobj::index_t idx0 = mesh.indices[index_offset + 0];
					tinyobj::index_t idx1 = mesh.indices[index_offset + 1];
					tinyobj::index_t idx2 = mesh.indices[index_offset + 2];
					calculateAndAssignNormal(
						m_vertices[idx0.vertex_index],
						m_vertices[idx1.vertex_index],
						m_vertices[idx2.vertex_index]
					);
				}

				index_offset += fv;
			}
		}
	}

	void Mesh::Unload() {
		m_vertices.clear();
		m_indices.clear();
	}

	void Mesh::calculateAndAssignNormal(Vertex& v0, Vertex& v1, Vertex& v2) {
		glm::vec3 normal = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
		v0.normal = normal;
		v1.normal = normal;
		v2.normal = normal;
	}
}