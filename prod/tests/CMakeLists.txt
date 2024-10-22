add_executable(test_main 
  test_main.cpp 
  ${CMAKE_SOURCE_DIR}/application/core.cpp
  ${CMAKE_SOURCE_DIR}/application/udp.cpp
)

target_include_directories(test_main
  PRIVATE
  ${CMAKE_SOURCE_DIR}/app
)

target_include_directories(test_main PRIVATE ${CMAKE_SOURCE_DIR}/application)

target_link_libraries(test_main PRIVATE 
    ${OpenCV_LIBS} 
    Qt6::Core 
    Qt6::Bluetooth
    Qt6::Network
)
