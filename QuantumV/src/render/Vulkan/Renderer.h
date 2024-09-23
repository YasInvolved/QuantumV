#pragma once

#include "../IRenderer.h"
#include "../../core/Window.h"

namespace QuantumV::Vulkan {
	class Renderer : virtual public IRenderer {
	public:
		Renderer(const Window* window);

		void Initialize() override;
		void Draw() override;
	private:
		const Window* m_window;
	};
}