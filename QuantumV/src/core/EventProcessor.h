#pragma once

#include <QuantumV/core/IEvent.h>
#include <QuantumV/core/EventQueue.h>
#include <QuantumV/core/EventDispatcher.h>
#include <thread>

namespace QuantumV {
	class EventProcessor {
	private:
		EventQueue& m_EventQueue;
		EventDispatcher& m_EventDispatcher;
	public:
		EventProcessor(EventQueue& eventQueue, EventDispatcher& eventDispatcher);
		void Start();
	};
}