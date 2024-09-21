#pragma once

#include "../core/Base.h"
#include <QuantumV/events/IEvent.h>

namespace QuantumV {
	class QV_API KeyboardEvent : public IEvent {
	public:
		KeyboardEvent(uint32_t scancode, bool isPressed);

		uint32_t GetScancode() const;
		bool GetIsPressed() const;
	private:
		uint32_t m_scancode;
		bool m_isPressed;
	};
}