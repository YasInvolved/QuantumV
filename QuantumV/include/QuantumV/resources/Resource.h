#pragma once

#include <QuantumV/core/Base.h>
#include <string>

namespace QuantumV {
	class QV_API Resource {
	public:
		virtual ~Resource() = default;
		virtual void Load(const std::string& filePath) = 0;
		virtual void Unload() = 0;
	};
}