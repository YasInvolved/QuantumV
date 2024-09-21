#include <QuantumV/core/EventDispatcher.h>

namespace QuantumV {
	void EventDispatcher::Dispatch(IEvent* event) {
		auto it = m_handlers.find(typeid(*event));
		if (it != m_handlers.end()) {
			it->second(event);
		}
	}
}