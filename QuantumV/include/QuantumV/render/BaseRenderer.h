#pragma once

#include <QuantumV/core/Base.h>

namespace QuantumV {
	// forward declare window class
	class Window;

	class QV_API BaseRenderer {
	public:
		BaseRenderer(Window& window) : m_window(window) {}
		virtual ~BaseRenderer() = default;

		virtual void Init() = 0;
		virtual void Render() = 0;
		virtual void Shutdown() = 0;
	protected:
		BaseRenderer() = delete; // disallow default constructor for deriving classes
		Window& m_window;
	};
}