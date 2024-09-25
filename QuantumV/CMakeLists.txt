if (QV_STATIC)
	message(STATUS "Static lib not supported yet")
else()
	add_library(QuantumV SHARED "src/core/Application.cpp" "include/QuantumV/core/Application.h" "include/QuantumV/entry_point.h" "include/QuantumV.h" "src/core/Window.cpp" "src/core/Window.h" "include/QuantumV/dll_header.h"   "include/QuantumV/core/Log.h" "src/core/Log.cpp" "include/QuantumV/core/Base.h" "src/utils/Timer.h" "src/utils/Timer.cpp" "include/QuantumV/events/IEvent.h" "include/QuantumV/events/EventQueue.h" "src/events/EventDispatcher.h" "src/events/EventDispatcher.cpp" "src/events/EventProcessor.h" "src/events/EventProcessor.cpp"  "src/events/EventQueue.cpp" "include/QuantumV/events/KeyboardEvent.h" "src/events/KeyboardEvent.cpp" "src/render/IRenderer.h" "src/render/IAllocator.h" "src/render/PlatformDependent.h" "src/render/D3D12/Renderer.h" "src/render/D3D12/Renderer.cpp" "src/render/RendererFactory.h" "src/render/RendererFactory.cpp" "src/render/Vulkan/Renderer.h" "src/render/Vulkan/Renderer.cpp" "src/render/D3D12/Allocator.cpp" "src/render/D3D12/Allocator.h")

	target_include_directories(QuantumV PUBLIC include)
	target_compile_definitions(QuantumV PUBLIC QV_SHARED PRIVATE QV_EXPORT $<$<CONFIG:Debug>:QV_DEBUG>) # public definitions
endif()

# platform dependent definitions
if (MSVC)
	target_compile_definitions(QuantumV PUBLIC QV_PLATFORM_WINDOWS QV_RENDERER_DX12)
	target_link_libraries(QuantumV PRIVATE "D3D12.lib" "dxgi.lib" "d3dcompiler.lib" D3D12MemoryAllocator)
endif()
if (LINUX)
	target_compile_definitions(QuantumV PUBLIC QV_PLATFORM_LINUX QV_RENDERER_VULKAN)
endif()

target_link_libraries(QuantumV PRIVATE SDL3::SDL3-shared imgui tinyobjloader glm::glm volk::volk PUBLIC spdlog::spdlog)

add_custom_target(CompileShaders)

set(SHADER_PATH_DX12 assets/shaders/dx12)
set(HLSL_SHADER_FILES ${SHADER_PATH_DX12}/VertexShader.hlsl ${SHADER_PATH_DX12}/PixelShader.hlsl)

set_source_files_properties(${SHADER_PATH_DX12}/VertexShader.hlsl PROPERTIES ShaderType "vs_5_0")
set_source_files_properties(${SHADER_PATH_DX12}/VertexShader.hlsl PROPERTIES EntryPoint "VSMain")

set_source_files_properties(${SHADER_PATH_DX12}/PixelShader.hlsl PROPERTIES ShaderType "ps_5_0")
set_source_files_properties(${SHADER_PATH_DX12}/PixelShader.hlsl PROPERTIES EntryPoint "PSMain")

# create shader build directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${SHADER_PATH_DX12})

foreach (FILE ${HLSL_SHADER_FILES})
	get_filename_component(FILE_WE ${FILE} NAME_WE)
	get_source_file_property(SHADER_TYPE ${FILE} ShaderType)
	get_source_file_property(ENTRY_POINT ${FILE} EntryPoint)
	add_custom_command(TARGET CompileShaders
    COMMAND dxc -T ${SHADER_TYPE} -E ${ENTRY_POINT} -Fo ${CMAKE_BINARY_DIR}/bin/${SHADER_PATH_DX12}/${FILE_WE}.cso ${FILE}
		MAIN_DEPENDENCY ${FILE}
		COMMENT "Building HLSL Shader: ${FILE}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM
	)
endforeach()

add_dependencies(QuantumV CompileShaders)