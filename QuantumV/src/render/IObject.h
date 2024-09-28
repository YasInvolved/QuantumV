#pragma once

#include <QuantumV/core/Base.h>
#include "IAllocator.h"
#include "PlatformDependent.h"
#include <string>
#include <glm/glm.hpp>

namespace QuantumV {
	class IObject {
	public:
		virtual ~IObject() = default;

		virtual VertexBufferHandle GetVertexBuffer() = 0;
		virtual IndexBufferHandle GetIndexBuffer() = 0;
		virtual BufferHandle GetConstantBuffer() = 0;
		virtual void SetPositon(float x, float y, float z) = 0;
		virtual void SetScale(float x, float y, float z) = 0;
	};
}