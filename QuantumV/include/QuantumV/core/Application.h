#pragma once

#include "Base.h"
#include <string>
#include <QuantumV/events/EventQueue.h>
#include <QuantumV/render/BaseRenderer.h>
#include <QuantumV/resources/ResourceManager.h>

namespace QuantumV {
	class Window;
	class EventDispatcher;

	class QV_API Application {
	public:
		Application();
		virtual ~Application();

		virtual void OnEvent(IEvent* event) {}
		void Run();
	protected:
		Scope<BaseRenderer> m_renderer;
		Scope<ResourceManager> m_resourceManager;
		void SetName(const std::string& name);
	private:
		std::string m_name = "QuantumClient";
		Scope<Window> m_window;
		Scope<EventQueue> m_eventQueue;
		Scope<EventDispatcher> m_dispatcher;
	};

	Application* CreateApplication();
}