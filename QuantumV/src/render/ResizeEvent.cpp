#include "ResizeEvent.h"

namespace QuantumV {
	ResizeEvent::ResizeEvent(IRenderer& renderer, uint32_t width, uint32_t height)
		: m_renderer(renderer), m_width(width), m_height(height) {}

	void ResizeEvent::OnEvent() {
		m_renderer.Resize(m_width, m_height);
	}
}