#pragma once

#include <QuantumV/core/Log.h>

#include "Math.h"
#include <string>
#include <vector>
#include <tiny_obj_loader.h>
#include <future>

namespace QuantumV {
	struct Vertex {
		glm::vec3 position;
		glm::vec4 color;
	};

	class Object {
	public:
		Object(const std::string& path);
		virtual ~Object() = default;

		void Load();
	protected:
		bool GetIsLoaded() const { return m_isLoaded; }
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
	private:
		bool m_isLoaded = false;
		tinyobj::ObjReader m_reader;
		const std::string m_path;
	};
}