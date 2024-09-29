#pragma once

#include "IObject.h"
// #include 
#include <future>
#include <QuantumV/core/Base.h>

namespace QuantumV {
	class ObjectFactory {
	public:
		static std::future<IObject*> CreateObjectAsync(const std::string& filepath, Ref<IAllocator> allocator);
	};
}