message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags, Debug configuration: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags, Release configuration: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Current C++ standard: ${CMAKE_CXX_STANDARD}")

set(OpenCV_STATIC ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
project(libcamera-demo)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAMERA REQUIRED libcamera)

find_library(LIBCAMERA_LIBRARY libcamera.so REQUIRED)
find_library(LIBCAMERA_BASE_LIBRARY libcamera-base.so REQUIRED)

find_package(OpenCV REQUIRED)

add_executable(application 
    app.cpp
    bluetoothDevice.cpp
    core.cpp 
    cv.cpp
    event.cpp
    LibCamera.cpp
    main.cpp
    network.cpp
)

target_include_directories(application PRIVATE 
    ${CAMERA_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS} 
    ${LIBCAMERA_INCLUDE_DIRS}
)

target_link_libraries(application PRIVATE 
    ${LIBCAMERA_BASE_LIBRARY}
    ${LIBCAMERA_LIBRARY}
    ${OpenCV_LIBS} 
    Qt6::Core 
    Qt6::Bluetooth
    Qt6::Network
    Qt6::Concurrent
)

if(NOT_BLE)
    target_compile_definitions(application PRIVATE NOT_BLE)
    message("NOT_BLE: ON")
else()
    message("NOT_BLE: OFF")
endif()
