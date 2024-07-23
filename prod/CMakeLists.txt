cmake_minimum_required(VERSION 3.5)

project(app VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenCV REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Bluetooth 
    Network
)

add_executable(main 
    main.cpp 
    bluetoothDevice.cpp
)

target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(main PRIVATE 
    ${OpenCV_LIBS} 
    Qt6::Core 
    Qt6::Bluetooth
    Qt6::Network
)
