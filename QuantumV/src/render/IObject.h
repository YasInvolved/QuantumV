#pragma once

#include <QuantumV/core/Base.h>
#include "IAllocator.h"
#include "PlatformDependent.h"
#include <string>
#include <glm/glm.hpp>
#include <optional>
#include <tiny_obj_loader.h>

namespace QuantumV {
	class IObject {
	public:
		virtual ~IObject() = default;

		virtual VertexBufferHandle GetVertexBuffer() = 0;
		virtual IndexBufferHandle GetIndexBuffer() = 0;
		virtual BufferHandle GetConstantBuffer() = 0;
		virtual void SetPositon(float x, float y, float z) = 0;
		virtual void SetScale(float x, float y, float z) = 0;
		virtual void Move(float delta_x, float delta_y, float delta_z) = 0;
		virtual void Scale(float delta_x, float delta_y, float delta_z) = 0;
		virtual void Rotate(float angle_x, float angle_y, float angle_z) = 0;
	};
}