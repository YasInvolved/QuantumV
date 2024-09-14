if (QV_STATIC)
	message(STATUS "Static lib not supported yet")
else()
	add_library(QuantumV SHARED  "src/core/Application.cpp" "include/QuantumV/core/Application.h" "include/QuantumV/entry_point.h" "include/QuantumV.h" "src/core/Window.cpp" "src/core/Window.h" "include/QuantumV/dll_header.h"  )
	target_include_directories(QuantumV PUBLIC include)
	target_compile_definitions(QuantumV PUBLIC QV_SHARED PRIVATE QV_EXPORT) # public definitions
endif()

# platform dependent definitions
if (MSVC)
	target_compile_definitions(QuantumV PUBLIC QV_PLATFORM_MSVC)
endif()

target_link_libraries(QuantumV PRIVATE SDL3::SDL3-shared PUBLIC spdlog::spdlog)