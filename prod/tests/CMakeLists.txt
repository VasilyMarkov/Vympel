# set(CMAKE_CXX_FLAGS_DEBUG "-g -O2 -Wall -Wextra -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(test_main 
  test_main.cpp 
  ${CMAKE_SOURCE_DIR}/app/core.cpp
  ${CMAKE_SOURCE_DIR}/app/udp.cpp
)

target_compile_features(test_main PUBLIC cxx_std_20)

target_include_directories(test_main
  PRIVATE
  ${CMAKE_SOURCE_DIR}/app
)

target_link_libraries(test_main PRIVATE 
    Qt5::Core 
    Qt5::Network
)