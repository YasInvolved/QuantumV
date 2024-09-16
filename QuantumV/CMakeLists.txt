if (QV_STATIC)
	message(STATUS "Static lib not supported yet")
else()
	add_library(QuantumV SHARED "src/core/Application.cpp" "include/QuantumV/core/Application.h" "include/QuantumV/entry_point.h" "include/QuantumV.h" "src/core/Window.cpp" "src/core/Window.h" "include/QuantumV/dll_header.h"   "include/QuantumV/core/Log.h" "src/core/Log.cpp" "include/QuantumV/core/Base.h" "src/render/IRenderer.h" "src/render/DirectX12Renderer.h" "src/render/DirectX12Renderer.cpp")
	target_include_directories(QuantumV PUBLIC include)
	target_compile_definitions(QuantumV PUBLIC QV_SHARED PRIVATE QV_EXPORT $<$<CONFIG:Debug>:QV_DEBUG>) # public definitions
endif()

# platform dependent definitions
if (MSVC)
	target_compile_definitions(QuantumV PUBLIC QV_PLATFORM_WINDOWS QV_RENDERER_DX12)
	target_link_libraries(QuantumV PRIVATE "D3D12.lib" "dxgi.lib" "d3dcompiler.lib")
endif()
if (LINUX)
	target_compile_definitions(QuantumV PUBLIC QV_PLATFORM_LINUX QV_RENDERER_VULKAN)
endif()

target_link_libraries(QuantumV PRIVATE SDL3::SDL3-shared PUBLIC spdlog::spdlog)