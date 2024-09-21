#include <QuantumV/events/KeyboardEvent.h>

namespace QuantumV {
	KeyboardEvent::KeyboardEvent(uint32_t scancode, bool isPressed)
		: m_scancode(scancode), m_isPressed(isPressed) {

	}

	uint32_t KeyboardEvent::GetScancode() const {
		return m_scancode;
	}

	bool KeyboardEvent::GetIsPressed() const {
		return m_isPressed;
	}
}