#pragma once

#include "Base.h"
#include <string>
#include <QuantumV/events/EventQueue.h>

namespace QuantumV {
	class Window;
	class EventDispatcher;
	class ICamera;

	class QV_API Application {
	public:
		Application();
		virtual ~Application();

		virtual void OnEvent(IEvent* event) {}
		void Run();
	protected:
		void SetName(const std::string& name);
	private:
		std::string m_name = "QuantumClient";
		Scope<Window> m_window;
		ICamera* m_camera;
		Ref<EventQueue> m_eventQueue;
		Scope<EventDispatcher> m_dispatcher;
	};

	Application* CreateApplication();
}