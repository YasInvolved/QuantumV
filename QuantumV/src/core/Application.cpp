#include "QuantumV/core/Application.h"
#include "QuantumV/core/Log.h"
#include <QuantumV/events/EventQueue.h>
#include <QuantumV/events/KeyboardEvent.h>
#include "../events/EventDispatcher.h"
#include "../events/EventProcessor.h"
#include "Window.h"
#include <iostream>
#include <imgui_impl_sdl3.h>

#ifdef QV_RENDERER_DX12
#include "../render/DX12Renderer.h"

namespace QuantumV {
	Application::Application() {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
		m_window = new Window(m_name, WindowType::BORDERLESS);
		QV_CORE_TRACE("Created application window: {0} {1}x{2}", m_name, m_window->getWidth(), m_window->getHeight());

		QV_CORE_TRACE("Creating event handlers");
		m_eventQueue = new EventQueue();
		m_dispatcher = new EventDispatcher(*this);

		QV_CORE_TRACE("Chosen renderer: DX12");
		m_renderer = new DX12Renderer();
		m_renderer->Init(m_window, m_window->getWidth(), m_window->getHeight());
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
				if (event.type == SDL_EVENT_QUIT) {
					running = false;
				}

				if ((event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) && !event.key.repeat) {
					m_eventQueue->PushEvent(std::make_unique<KeyboardEvent>(event.key.key, event.key.down));
				}

				ImGui_ImplSDL3_ProcessEvent(&event);
			}
			m_renderer->Draw(0, 0);
		}

		processor.Stop();
	}

	void Application::SetName(const std::string& name) {
		m_name = name;
	}
}

#endif