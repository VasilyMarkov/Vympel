message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags, Debug configuration: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags, Release configuration: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Current C++ standard: ${CMAKE_CXX_STANDARD}")

add_executable(application
    bleService.cpp
    bluetoothDevice.cpp
    main.cpp
    udp.cpp
)

target_include_directories(application PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(application PRIVATE 
    Qt6::Core 
    Qt6::Bluetooth
    Qt6::Network
    Qt6::Concurrent
)
