#pragma once

#include <QuantumV/core/IEvent.h>
#include <unordered_map>
#include <functional>
#include <typeindex>

namespace QuantumV {
	using EventHandlerFunction = std::function<void(IEvent*)>;

	class EventDispatcher {
	private:
		std::unordered_map<std::type_index, EventHandlerFunction> m_handlers;

	public:
		template<typename EventType>
		void RegisterHandler(std::function<void(EventType*)> handler) {
			m_handlers[typeid(EventType)] = [handler](IEvent* event) {
				handler(static_cast<EventType*>(event));
			};
		}
		
		void Dispatch(IEvent* event);
	};
}