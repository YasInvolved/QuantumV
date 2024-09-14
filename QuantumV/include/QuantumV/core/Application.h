#pragma once

#include "../dll_header.h"

namespace QuantumV {
	class Window;

	class QV_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		Window* window;
	};

	Application* CreateApplication();
}