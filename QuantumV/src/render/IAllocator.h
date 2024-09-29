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
		virtual ImageHandle AllocateImage(size_t width, size_t height) = 0;
		virtual void GenerateMemoryDump() = 0;
		virtual RenderAPI GetRenderAPI() = 0;
	};
}