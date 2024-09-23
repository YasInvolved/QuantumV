#pragma once

#include <QuantumV/core/Base.h>
#include "IRenderer.h"
#include "PlatformDependent.h"
#include "../core/Window.h"

namespace QuantumV {
	class RendererFactory {
	public:
		static Scope<IRenderer> CreateRenderer(const Window* window, RenderAPI renderapi);
	};
}