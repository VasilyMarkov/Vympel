include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
message("Tests building...")

add_executable(tests tests.cpp)

target_include_directories(tests
  PRIVATE
  ${CMAKE_SOURCE_DIR}/app
)

target_link_libraries(tests PRIVATE  
    Qt6::Core 
    Qt6::Bluetooth
    Qt6::Network
)

target_link_libraries(tests PRIVATE gtest_main gmock_main)