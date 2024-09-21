#include "EventDispatcher.h"

namespace QuantumV {
	EventDispatcher::EventDispatcher(Application& application)
		: m_application(application) {}

	void EventDispatcher::Dispatch(IEvent* event) {
		if (event) {
			event->OnEvent();
			m_application.OnEvent(event);
		}
	}
}