cmake_minimum_required(VERSION 3.5)
project(ComputerVision VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Bluetooth 
    Network
    Concurrent
)

add_subdirectory(application)
