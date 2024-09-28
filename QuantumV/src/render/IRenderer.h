#pragma once

#include <QuantumV/core/Base.h>
#include "IAllocator.h"
#include "IObject.h"
#include "ICamera.h"

namespace QuantumV {
	class IRenderer {
	public:
		virtual ~IRenderer() {};

		virtual void Initialize() = 0;
		virtual void Draw() = 0;
		virtual IAllocator* GetAllocator() = 0;
		virtual void AddObject(IObject* object) = 0;
		virtual void AddCamera(ICamera* camera) = 0;
	};
}