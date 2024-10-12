#pragma once

#include <QuantumV/resources/Resource.h>

namespace QuantumV {
	class QV_API Texture : public Resource {
		void Load(const std::string& filePath) override;
		void Unload() override;
	};
}