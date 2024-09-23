#pragma once

#include "IRenderer.h"
#include <string>

namespace QuantumV {
	class IObject {
	public:
		virtual ~IObject() {}

		virtual void Load() = 0;
		virtual void Create() = 0;
		virtual void Destroy() = 0;
		//virtual void Bind() = 0;
		//virtual void UnBind() = 0;

		static IObject* CreateObject(IRenderer* renderer, const std::string& path);
	};
}