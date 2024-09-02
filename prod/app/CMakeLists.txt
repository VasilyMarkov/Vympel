

# set(CMAKE_CXX_FLAGS_DEBUG "-g -O2 -Wall -Wextra -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags, Debug configuration: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags, Release configuration: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Current C++ standard: ${CMAKE_CXX_STANDARD}")

set(OpenCV_STATIC ON)

find_package(OpenCV REQUIRED)

add_executable(main 
    main.cpp
    core.cpp 
    cv.cpp
    bluetoothDevice.cpp
    udp.cpp
)

target_compile_features(main PUBLIC cxx_std_23)

target_include_directories(main PRIVATE 
    ${OpenCV_INCLUDE_DIRS} 
)

target_link_libraries(main PRIVATE 
    ${OpenCV_LIBS} 
    Qt6::Core 
    Qt6::Bluetooth
    Qt6::Network
)
