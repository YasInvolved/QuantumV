cmake_minimum_required(VERSION 3.20)

project(QuantumV)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_BIN}/lib")

option(QV_STATIC "Build static library" OFF)
option(QV_BUILD_SANDBOX "Build QuantumSandbox executable" ON)

find_package(Vulkan REQUIRED)

# dependencies
add_subdirectory(3rdparty/SDL3)
add_subdirectory(3rdparty/spdlog)
add_subdirectory(3rdparty/D3D12MemoryAllocator)
add_subdirectory(3rdparty/tinyobjloader)
add_subdirectory(3rdparty/glm)
add_subdirectory(3rdparty/volk)

add_library(imgui STATIC "3rdparty/imgui/imgui.cpp" "3rdparty/imgui/imgui_demo.cpp" "3rdparty/imgui/imgui_draw.cpp" "3rdparty/imgui/imgui_tables.cpp" "3rdparty/imgui/imgui_widgets.cpp" "3rdparty/imgui/backends/imgui_impl_dx12.cpp" "3rdparty/imgui/backends/imgui_impl_dx12.cpp" "3rdparty/imgui/backends/imgui_impl_sdl3.cpp")
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui" "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/backends")
target_link_libraries(imgui PRIVATE SDL3::Headers)

# platform dependent dependencies

add_subdirectory(QuantumV)
add_subdirectory(QuantumSandbox)