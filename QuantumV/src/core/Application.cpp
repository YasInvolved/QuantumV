#include "QuantumV/core/Application.h"
#include "Window.h"
#include <iostream>

namespace QuantumV {
	Application::Application() {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
		this->window = new Window("QuantumV", WindowType::WINDOWED);
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
		}
	}
}