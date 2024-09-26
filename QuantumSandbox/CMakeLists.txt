if(QV_BUILD_SANDBOX)
    add_executable(QuantumSandbox "main.cpp")

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(QuantumSandbox PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()

    target_link_libraries(QuantumSandbox PRIVATE QuantumV)
endif()