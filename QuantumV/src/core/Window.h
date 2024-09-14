#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace QuantumV {
	enum class WindowType {
		WINDOWED, BORDERLESS, FULLSCREEN
	};

	class Window {
	public:
		Window(const std::string& title, WindowType type);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

	private:
		SDL_Window* window_ptr;
		std::string title;
		uint32_t width;
		uint32_t height;
		uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
		WindowType type;
	};
}