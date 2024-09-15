#pragma once

#include "../dll_header.h"

#include <memory>

namespace QuantumV {
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;
}