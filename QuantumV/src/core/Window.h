#pragma once

#include <SDL3/SDL.h>
#include <string>

#ifdef QV_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

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

		uint32_t getWidth() { return width; }
		uint32_t getHeight() { return height; }
		#ifdef QV_PLATFORM_WINDOWS
		HWND getHWND() const { return reinterpret_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(this->window_ptr), SDL_PROP_WINDOW_WIN32_HWND_POINTER, 0)); }
		#endif
		SDL_Window* getSDLWindow() const { return window_ptr; }
	private:
		SDL_Window* window_ptr;
		std::string title;
		uint32_t width;
		uint32_t height;
		uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY/* | SDL_WINDOW_RESIZABLE */; // remove due to buggy resize on dx12
		WindowType type;
	};
}