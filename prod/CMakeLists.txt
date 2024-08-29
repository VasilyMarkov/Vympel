cmake_minimum_required(VERSION 3.5)


project(ComputerVision VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)


find_package(Qt5 REQUIRED COMPONENTS 
    Core 
    Network
)

add_subdirectory(app)
