#pragma once

#include <QuantumV/render/BaseRenderer.h>

namespace QuantumV::Vulkan {
	class QV_API Renderer : public BaseRenderer {
	public:
		Renderer(Window& window);
		~Renderer() override;

		void Init() override;
		void Render() override;
		void Shutdown() override;
	};
}