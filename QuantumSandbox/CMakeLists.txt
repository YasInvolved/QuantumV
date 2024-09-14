if(QV_BUILD_SANDBOX)

add_executable(QuantumSandbox "main.cpp")
target_link_libraries(QuantumSandbox PRIVATE QuantumV)

endif()