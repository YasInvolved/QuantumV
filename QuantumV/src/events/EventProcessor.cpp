#include "EventProcessor.h"

namespace QuantumV {
	EventProcessor::EventProcessor(EventQueue& eventQueue, EventDispatcher& eventDispatcher)
		: m_EventQueue(eventQueue), m_EventDispatcher(eventDispatcher) {}

	void EventProcessor::Start() {
		m_processorThread = std::thread([this]() {
			while (true) {
				auto event = m_EventQueue.PopEvent();
				if (event) {
					m_EventDispatcher.Dispatch(event.get());
				}
			}
		});
		m_processorThread.detach();
	}

	void EventProcessor::Stop() {
		if (m_processorThread.joinable()) {
			m_processorThread.join();
		}
	}
}