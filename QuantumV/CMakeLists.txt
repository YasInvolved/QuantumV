if (QV_STATIC)
	message(STATUS "Static lib not supported yet")
else()
	add_library(QuantumV SHARED 
		"src/core/Application.cpp" "include/QuantumV/core/Application.h" 
		"include/QuantumV/entry_point.h" "include/QuantumV.h" 
		"src/core/Window.cpp" "src/core/Window.h" 
		"include/QuantumV/dll_header.h" "include/QuantumV/core/Log.h" 
		"src/core/Log.cpp" "include/QuantumV/core/Base.h" 
		"src/utils/Timer.h" "src/utils/Timer.cpp" 
		"include/QuantumV/events/IEvent.h"
		"include/QuantumV/events/EventQueue.h" 
		"src/events/EventDispatcher.h" 
		"src/events/EventDispatcher.cpp" 
		"src/events/EventProcessor.h" 
		"src/events/EventProcessor.cpp" 
		"src/events/EventQueue.cpp" 
		"src/utils/stb_image.h"
	  "include/QuantumV/render/BaseRenderer.h" "include/QuantumV/render/D3D12/D3D12Renderer.h" "src/render/D3D12/D3D12Renderer.cpp" "include/QuantumV/render/Vulkan/VulkanRenderer.h" "src/render/Vulkan/VulkanRenderer.cpp" "include/QuantumV/resources/Resource.h" "include/QuantumV/resources/Texture.h" "include/QuantumV/resources/Mesh.h" "include/QuantumV/resources/ResourceManager.h" "src/resources/ResourceManager.cpp" "src/resources/Mesh.cpp" "src/resources/Texture.cpp")

	target_include_directories(QuantumV PUBLIC include)
	target_compile_definitions(QuantumV PUBLIC QV_SHARED $<$<CONFIG:Debug>:QV_DEBUG> PRIVATE QV_EXPORT) # public definitions
endif()

# platform dependent definitions
if (MSVC)
	target_compile_definitions(QuantumV PUBLIC QV_PLATFORM_WINDOWS)
	target_link_libraries(QuantumV PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib" "dxguid.lib" D3D12MemoryAllocator)
endif()
if (LINUX)
	target_compile_definitions(QuantumV PUBLIC QV_PLATFORM_LINUX QV_RENDERER_VULKAN)
endif()

target_link_libraries(QuantumV PRIVATE SDL3::SDL3-shared imgui tinyobjloader glm::glm volk::volk crossguid PUBLIC spdlog::spdlog)