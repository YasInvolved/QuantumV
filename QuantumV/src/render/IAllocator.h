#pragma once

#include "PlatformDependent.h"
#include <variant>

namespace QuantumV {
	class IAllocator {
	public:
		virtual ~IAllocator() = default;

		virtual BufferHandle AllocateBuffer(size_t size) = 0;
		virtual VertexBufferHandle AllocateVertexBuffer(size_t size) = 0;
		virtual IndexBufferHandle AllocateIndexBuffer(size_t size) = 0;
	};
}