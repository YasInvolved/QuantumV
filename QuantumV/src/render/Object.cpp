#include "Object.h"

namespace QuantumV {
	Object::Object(const std::string& path)
		: m_path(path)
	{
		tinyobj::ObjReaderConfig readerConfig;
		readerConfig.mtl_search_path = "./";

		if (!m_reader.ParseFromFile(path, readerConfig)) {
			if (!m_reader.Error().empty()) {
				QV_CORE_ERROR("Failed to load model {}", path);
				return;
			}
		}

		if (!m_reader.Warning().empty()) {
			QV_CLIENT_WARN("TinyObjLoader: {}", m_reader.Warning());
		}
	}

	void Object::Load() {
		const tinyobj::attrib_t& attrib = m_reader.GetAttrib();
		const std::vector<tinyobj::shape_t>& shapes = m_reader.GetShapes();
		const std::vector<tinyobj::material_t>& materials = m_reader.GetMaterials();

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

					m_vertices.push_back(Vertex{ {vx, vy, vz}, {0.52f, 0.52f, 0.52f, 1.0f} }); // dark gray
					m_indices.push_back(index_offset + v);
					QV_CORE_TRACE("Vertex: x:{0} y:{1} z:{2}", vx, vy, vz);
					QV_CORE_TRACE("Index: {}", index_offset + v);

					if (idx.normal_index >= 0) {
						tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
						tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
						tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
					}

					if (idx.texcoord_index >= 0) {
						tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
						tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					}
				}

				index_offset += fv;
			}
		}

		m_isLoaded = true;
	}
}