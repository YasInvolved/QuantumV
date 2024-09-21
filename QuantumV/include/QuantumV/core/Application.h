#pragma once

#include "Base.h"
#include <string>
#include <QuantumV/events/EventQueue.h>

namespace QuantumV {
	class Window;
	class IRenderer;
	class EventDispatcher;

	class QV_API Application {
	public:
		Application();
		virtual ~Application();

		virtual void OnEvent(IEvent* event) {}
		void Run();
	protected:
		void SetName(const std::string& name);
		EventQueue* m_eventQueue;
		EventDispatcher* m_dispatcher;
	private:
		std::string m_name = "QuantumClient";
		Window* m_window;
		IRenderer* m_renderer;
	};

	Application* CreateApplication();
}