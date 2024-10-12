#pragma once

#include <QuantumV/resources/Resource.h>

namespace QuantumV {
	class Mesh : public Resource {
	public:
		void Load(const std::string& filePath) override;
		void Unload() override;
	};
}