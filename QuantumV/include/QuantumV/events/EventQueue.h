#pragma once

#include <QuantumV/events/IEvent.h>
#include <queue>
#include <memory>
#include <mutex>

namespace QuantumV {
	// Thread-safe event queue
	class EventQueue {
	private:
		std::queue<std::unique_ptr<IEvent>> m_queue;
		std::mutex m_queueMutex;
		std::condition_variable m_condition;

	public:
		// prevent copying
		EventQueue(const EventQueue&) = delete;
		EventQueue& operator=(const EventQueue&) = delete;

		void PushEvent(std::unique_ptr<IEvent> event);
		std::unique_ptr<IEvent> PopEvent();
	};
}