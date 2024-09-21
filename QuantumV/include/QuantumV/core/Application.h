#pragma once

#include "Base.h"
#include <string>
#include <QuantumV/core/EventQueue.h>
#include <QuantumV/core/EventDispatcher.h>

namespace QuantumV {
	class Window;
	class IRenderer;

	class QV_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::string m_name = "QuantumClient";
		Window* m_window;
		EventQueue* m_eventQueue;
		EventDispatcher* m_dispatcher;
		IRenderer* m_renderer;
	};

	Application* CreateApplication();
}