#pragma once

#include "RenderAPI.h"
#include <stdint.h>
#include "../core/Window.h"

namespace QuantumV {
	class IRenderer {
	public:
		virtual ~IRenderer() {}

		virtual void Init() = 0;
		virtual void RenderFrame() = 0;
		virtual void Shutdown() = 0;
		virtual void Resize(uint32_t new_width, uint32_t new_height) = 0;
		virtual RenderAPI GetRenderAPI() const = 0;

		static IRenderer* CreateRenderer(RenderAPI api, Window* window);
	};
}