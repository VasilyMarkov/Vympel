cmake_minimum_required(VERSION 3.5)
project(ComputerVision VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Bluetooth 
    Network
    SerialPort
    SerialBus
)

add_subdirectory(app)
# add_subdirectory(tests)


# if(TESTS)
#     message("TESTS ON")
#     enable_testing()
#     add_subdirectory(tests)
# endif(TESTS)