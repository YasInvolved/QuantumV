#include "QuantumV/core/Application.h"
#include "QuantumV/core/Log.h"
#include <QuantumV/events/EventQueue.h>
#include "Window.h"
#include "../events/EventDispatcher.h"
#include "../events/EventProcessor.h"

namespace QuantumV {
	Application::Application() {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
		m_window = std::make_unique<Window>(m_name, WindowType::WINDOWED);
		QV_CORE_TRACE("Created application window: {0} {1}x{2}", m_name, m_window->getWidth(), m_window->getHeight());

		QV_CORE_TRACE("Creating event handlers");
		m_eventQueue = std::make_unique<EventQueue>();
		m_dispatcher = std::make_unique<EventDispatcher>(*this);
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
					if (event.key.key == SDLK_ESCAPE) {
						running = false;
						break;
					}
					break;

				case SDL_EVENT_WINDOW_RESIZED:
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