#pragma once

#include "IObject.h"
#include <future>
#include <QuantumV/core/Base.h>
#include <optional>

namespace QuantumV {
	class ObjectFactory {
	public:
		static std::future<Ref<IObject>> CreateObjectAsync(const std::string& filepath, const Ref<IAllocator> allocator, std::optional<const std::string> materialPath = std::nullopt);
	};
}