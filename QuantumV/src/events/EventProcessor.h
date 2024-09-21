#pragma once

#include <QuantumV/events/IEvent.h>
#include <QuantumV/events/EventQueue.h>
#include "EventDispatcher.h"
#include <thread>

namespace QuantumV {
	class EventProcessor {
	private:
		EventQueue& m_EventQueue;
		EventDispatcher& m_EventDispatcher;

		std::thread m_processorThread;
	public:
		EventProcessor(EventQueue& eventQueue, EventDispatcher& eventDispatcher);
		~EventProcessor() { Stop(); }
		void Start();
		void Stop();
	};
}