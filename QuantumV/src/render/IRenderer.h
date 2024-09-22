#pragma once

#include "RenderAPI.h"

namespace QuantumV {
	class IRenderer {
	public:
		virtual ~IRenderer() {}

		virtual void Init() = 0;
		virtual void RenderFrame() = 0;
		virtual void Shutdown() = 0;

		static IRenderer* CreateRenderer(RenderAPI api);
	};
}