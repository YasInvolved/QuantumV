#include "Window.h"
#include <iostream>

namespace QuantumV {
	Window::Window(const std::string& title, WindowType type)
		: title(title), type(type)
	{
		switch (type) {
		case WindowType::WINDOWED:
			break;
		case WindowType::BORDERLESS:
			flags |= SDL_WINDOW_BORDERLESS;
			break;
		case WindowType::FULLSCREEN:
			flags |= SDL_WINDOW_FULLSCREEN;
			break;
		}

		auto display_mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
		this->width = display_mode->w / 2;
		this->height = display_mode->h / 2;

		this->window_ptr = SDL_CreateWindow(title.c_str(), this->width, this->height, flags);
	}

	Window::~Window() {
		SDL_DestroyWindow(this->window_ptr);
	}
}