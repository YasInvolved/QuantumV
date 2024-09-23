#pragma once

#include "IAllocator.h"

namespace QuantumV {
	class IRenderer {
	public:
		virtual ~IRenderer() {};

		virtual void Initialize() = 0;
		virtual void Draw() = 0;
		// virtual IAllocator GetAllocator() = 0;
	};
}