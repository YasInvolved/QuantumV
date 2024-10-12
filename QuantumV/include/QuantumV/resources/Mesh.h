#pragma once

#include <QuantumV/resources/Resource.h>
#include <vector>
#include <glm/glm.hpp>

namespace QuantumV {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 color;
	};

	class QV_API Mesh : public Resource {
	private:
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
	public:
		void Load(const std::string& filePath) override;
		void Unload() override;
	private:
		void calculateAndAssignNormal(Vertex& v0, Vertex& v1, Vertex& v2);
	};
}