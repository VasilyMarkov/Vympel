include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
message("Tests building...")

add_executable(unit_tests 
  unit_tests.cpp 
)

target_include_directories(unit_tests
  PRIVATE
  ${CMAKE_SOURCE_DIR}/app
  ${CMAKE_SOURCE_DIR}/tests
)

target_link_libraries(unit_tests PRIVATE gtest_main gmock_main)

target_link_libraries(unit_tests PRIVATE 
    Qt6::Core 
    Qt6::Bluetooth
    Qt6::Network
)
