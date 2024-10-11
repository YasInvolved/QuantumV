#pragma once

#include "../dll_header.h"

#include <memory>

namespace QuantumV {
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	enum class GraphicsAPI {
		D3D12, VULKAN //, OPENGL 
	};

#ifdef QV_PLATFORM_WINDOWS
	constexpr GraphicsAPI preferredAPI = GraphicsAPI::D3D12;
#else
	constexpr GraphicsAPI preferredAPI = GraphicsAPI::VULKAN;
#endif
}