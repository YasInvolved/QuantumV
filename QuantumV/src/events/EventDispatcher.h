#pragma once

#include <QuantumV/core/Application.h>
#include <QuantumV/events/IEvent.h>
#include <unordered_map>
#include <functional>
#include <typeindex>

namespace QuantumV {
	using EventHandlerFunction = std::function<void(IEvent*)>;

	class EventDispatcher {
	private:
		Application& m_application;
		std::unordered_map<std::type_index, EventHandlerFunction> m_handlers;

	public:
		EventDispatcher(Application& application);

		void Dispatch(IEvent* event);
	};
}