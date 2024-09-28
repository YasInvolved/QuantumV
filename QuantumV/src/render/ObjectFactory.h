#pragma once

#include "IObject.h"
// #include 
#include <future>

namespace QuantumV {
	class ObjectFactory {
	public:
		static std::future<IObject*> CreateObjectAsync(const std::string& filepath, IAllocator* allocator);
	};
}