#include "QuantumV/core/Application.h"
#include "QuantumV/core/Log.h"
#include "Window.h"
#include <iostream>

#ifdef QV_RENDERER_DX12
#include "../render/DX12/DX12Renderer.h"
#endif

namespace QuantumV {
	Application::Application() {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
		this->window = new Window(this->name, WindowType::WINDOWED);
		QV_CORE_TRACE("Created application window: {0} {1}x{2}", this->name, this->window->getWidth(), this->window->getHeight());
		
		QV_CORE_TRACE("Chosen renderer: DX12");
		this->renderer = new DX12Renderer();
		this->renderer->Init(this->window->getHWND(), this->window->getWidth(), this->window->getHeight());
	}

	Application::~Application() {
		SDL_Quit();
	}

	void Application::Run() {
		bool running = true;
		SDL_Event event;
		while (running) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_EVENT_QUIT) {
					running = false;
				}
			}
			this->renderer->Draw(0, 0);
		}
	}
}