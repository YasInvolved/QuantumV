#pragma once

#include "IRenderer.h"
#include <QuantumV/events/IEvent.h>

namespace QuantumV {
	class ResizeEvent : public IEvent {
	public:
		ResizeEvent(IRenderer& renderer, uint32_t width, uint32_t height);

		void OnEvent() override;
	private:
		IRenderer& m_renderer;
		uint32_t m_width;
		uint32_t m_height;
	};
}