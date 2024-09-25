#include "QuantumV/core/Application.h"
#include "QuantumV/core/Log.h"
#include <QuantumV/events/EventQueue.h>
#include <QuantumV/events/KeyboardEvent.h>
#include "../events/EventDispatcher.h"
#include "../events/EventProcessor.h"
#include "Window.h"
#include "../render/RendererFactory.h"

namespace QuantumV {
	Application::Application() {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
		m_window = new Window(m_name, WindowType::WINDOWED);
		QV_CORE_TRACE("Created application window: {0} {1}x{2}", m_name, m_window->getWidth(), m_window->getHeight());

		QV_CORE_TRACE("Creating event handlers");
		m_eventQueue = new EventQueue();
		m_dispatcher = new EventDispatcher(*this);

		m_renderer = RendererFactory::CreateRenderer(m_window, preferredApi);
		m_renderer->Initialize();
	}

	Application::~Application() {
		SDL_Quit();
	}

	void Application::Run() {
		bool running = true;
		SDL_Event event;

		// create and start async event processor
		EventProcessor processor(*m_eventQueue, *m_dispatcher);
		processor.Start();

		while (running) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_EVENT_QUIT:
					running = false;
					break;

				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP:
					m_eventQueue->PushEvent(
						std::make_unique<KeyboardEvent>(
							event.key.key, event.key.down
						)
					);
					break;

				case SDL_EVENT_WINDOW_RESIZED:
					/*m_eventQueue->PushEvent(
						std::make_unique<ResizeEvent>(
							*m_renderer,
							static_cast<uint32_t>(event.display.data1),
							static_cast<uint32_t>(event.display.data2)
						)
					);*/
					break;
				}
			}
		}

		processor.Stop();
	}

	void Application::SetName(const std::string& name) {
		m_name = name;
	}
}