#include "QuantumV/core/Application.h"
#include "QuantumV/core/Log.h"
#include <QuantumV/core/EventDispatcher.h>
#include "EventProcessor.h"
#include "Window.h"
#include <iostream>
#include <imgui_impl_sdl3.h>

#ifdef QV_RENDERER_DX12
#include "../render/DX12Renderer.h"

namespace QuantumV {
	// Test event
	// TODO: Delete
	class KeyboardEvent : public IEvent {
	public:
		KeyboardEvent(uint32_t scancode, bool down) : m_scancode(scancode), m_down(down) {}

		const bool GetIsDown() const {
			return m_down;
		}

		const uint16_t GetScancode() const {
			return m_scancode;
		}

		const char* GetName() const override {
			return "KeyboardEvent";
		}
	private:
		bool m_down;
		uint32_t m_scancode;
	};

	static void handleKeyboardEvent(KeyboardEvent* event) {
		QV_CORE_TRACE("{} has been {}", event->GetScancode(), event->GetIsDown() ? "pressed" : "released");
	}

	Application::Application() {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
		m_window = new Window(m_name, WindowType::BORDERLESS);
		QV_CORE_TRACE("Created application window: {0} {1}x{2}", m_name, m_window->getWidth(), m_window->getHeight());

		QV_CORE_TRACE("Creating event handlers");
		m_eventQueue = new EventQueue();
		m_dispatcher = new EventDispatcher();

		m_dispatcher->RegisterHandler<KeyboardEvent>(handleKeyboardEvent);

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
		EventProcessor processor(*m_eventQueue, *m_dispatcher);
		processor.Start();
		while (running) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_EVENT_QUIT) {
					running = false;
				}
				
				if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
					if (!event.key.repeat) {
						m_eventQueue->PushEvent(
							std::make_unique<KeyboardEvent>(event.key.key, event.type == SDL_EVENT_KEY_DOWN)
						);
					}
				}
				ImGui_ImplSDL3_ProcessEvent(&event); // TODO: Fix this it's only temporary
			}
			m_renderer->Draw(0, 0);
		}
	}
}

#endif