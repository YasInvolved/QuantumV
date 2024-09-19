cmake_minimum_required(VERSION 3.20)

project(QuantumV)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_BIN}/lib")

option(QV_STATIC "Build static library" OFF)
option(QV_BUILD_SANDBOX "Build QuantumSandbox executable" ON)

# dependencies
add_subdirectory(3rdparty/SDL3)
add_subdirectory(3rdparty/spdlog)
add_subdirectory(3rdparty/D3D12MemoryAllocator)

# platform dependent dependencies

add_subdirectory(QuantumV)
add_subdirectory(QuantumSandbox)