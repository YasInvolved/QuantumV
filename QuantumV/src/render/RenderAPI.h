#pragma once

namespace QuantumV {
	enum class RenderAPI {
		DX12,
		VULKAN
	};
	namespace Globals {
		#ifdef QV_PLATFORM_WINDOWS
		constexpr RenderAPI preferredRenderingApi = RenderAPI::DX12;
		#else
		constexpr RenderAPI preferredRenderingApi = VULKAN
		#endif
	}
}