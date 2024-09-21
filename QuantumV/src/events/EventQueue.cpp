#include <QuantumV/events/EventQueue.h>

namespace QuantumV {
	void EventQueue::PushEvent(std::unique_ptr<IEvent> event) {
		std::lock_guard<std::mutex> lock(m_queueMutex);
		m_queue.push(std::move(event));
		m_condition.notify_one();
	}

	std::unique_ptr<IEvent> EventQueue::PopEvent() {
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_condition.wait(lock, [&]() { return !m_queue.empty(); });
		auto event = std::move(m_queue.front());
		m_queue.pop();
		return event;
	}
}