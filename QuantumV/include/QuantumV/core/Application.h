#pragma once

#include "Base.h"
#include <string>

namespace QuantumV {
	class Window;
	class IRenderer;

	class QV_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::string name = "QuantumClient";
		Window* window;
		IRenderer* renderer;
	};

	Application* CreateApplication();
}