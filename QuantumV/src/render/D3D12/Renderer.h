#ifdef QV_PLATFORM_WINDOWS

#pragma once

#include "../IRenderer.h"
#include "../../core/Window.h"

namespace QuantumV::D3D12 {
	class Renderer : virtual public IRenderer {
	public:
		Renderer(const Window* window);

		void Initialize() override;
		void Draw() override;
	private:
		const Window* m_window;
	};
}

#endif