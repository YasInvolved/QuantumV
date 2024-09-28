#pragma once

#include "PlatformDependent.h"

namespace QuantumV {
	class ICamera {
	public:
		virtual ~ICamera() = default;

		virtual BufferHandle GetCameraBuffer() = 0;
	};
}