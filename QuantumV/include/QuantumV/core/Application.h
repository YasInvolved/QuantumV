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
		std::string m_name = "QuantumClient";
		Window* m_window;
		IRenderer* m_renderer;
	};

	Application* CreateApplication();
}