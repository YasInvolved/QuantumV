#pragma once

#include <QuantumV/core/IEvent.h>
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
		void PushEvent(std::unique_ptr<IEvent> event) {
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_queue.push(std::move(event));
			m_condition.notify_one();
		}

		std::unique_ptr<IEvent> PopEvent() {
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_condition.wait(lock, [&]() { return !m_queue.empty(); } );
			auto event = std::move(m_queue.front());
			m_queue.pop();
			return event;
		}
	};
}