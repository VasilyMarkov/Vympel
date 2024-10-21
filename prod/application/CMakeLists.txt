message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags, Debug configuration: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags, Release configuration: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Current C++ standard: ${CMAKE_CXX_STANDARD}")

set(OpenCV_STATIC ON)

find_package(OpenCV REQUIRED)

add_executable(application 
    app.cpp
    bluetoothDevice.cpp
    core.cpp 
    cv.cpp
    event.cpp
    fsm.cpp
    LibCamera.cpp
    main.cpp
    udp.cpp
)

target_include_directories(application PRIVATE 
    ${OpenCV_INCLUDE_DIRS} 
    ${LIBCAMERA_INCLUDE_DIRS}
)

target_link_libraries(application PRIVATE 
    ${OpenCV_LIBS} 
    Qt5::Core 
    Qt5::Bluetooth
    Qt5::Network
)
