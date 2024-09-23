#pragma once

#include "PlatformDependent.h"
#include <variant>

namespace QuantumV {
	class IAllocator {
		virtual ~IAllocator() = default;

		virtual BufferHandle AllocateBuffer(size_t size) = 0;
		virtual BufferHandle AllocateVertexBuffer(size_t size) = 0;
		virtual BufferHandle AllocateIndexBuffer(size_t size) = 0;
	};
}