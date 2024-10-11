#ifdef QV_PLATFORM_WINDOWS // guard in case the build target was linux or mac

#pragma once
#include <QuantumV/render/BaseRenderer.h>

namespace QuantumV::D3D12 {
	class QV_API Renderer : public BaseRenderer {
	public:
		Renderer(Window& window);
		~Renderer() override;

		void Init() override;
		void Render() override;
		void Shutdown() override;
	};
}

#endif