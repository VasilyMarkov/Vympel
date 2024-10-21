



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


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
project(libcamera-demo)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAMERA REQUIRED libcamera)

find_library(LIBCAMERA_LIBRARY libcamera.so REQUIRED)
find_library(LIBCAMERA_BASE_LIBRARY libcamera-base.so REQUIRED)
message(STATUS ${CAMERA_INCLUDE_DIRS})
message(STATUS ${LIBCAMERA_LIBRARY})
message(STATUS ${LIBCAMERA_BASE_LIBRARY})

find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
	message("Found OpenCV")
	message("Includes:" ${OpenCV_INCLUDE_DIRS})
endif(OpenCV_FOUND)

include_directories(. "${CAMERA_INCLUDE_DIRS}")
set(LIBCAMERA_LIBRARIES "${LIBCAMERA_LIBRARY}" "${LIBCAMERA_BASE_LIBRARY}")
add_executable(main
    main.cpp 
    LibCamera.cpp
    core.cpp 
    cv.cpp
    udp.cpp
)

target_link_libraries(main
    "${LIBCAMERA_LIBRARIES}" 
    ${OpenCV_LIBS}
    Qt5::Core 
    Qt5::Bluetooth 
    Qt5::Network
)




# set(OpenCV_STATIC ON)

# find_package(OpenCV REQUIRED)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(LIBCAMERA REQUIRED libcamera)

# message(STATUS "libcamera library found:")
# message(STATUS "    version: ${LIBCAMERA_VERSION}")
# message(STATUS "    libraries: ${LIBCAMERA_LINK_LIBRARIES}")
# message(STATUS "    include path: ${LIBCAMERA_INCLUDE_DIRS}")

# For resolving names between Qt and libcamera
set(QT_NO_KEYWORDS ON)

# add_executable(main 
#     main.cpp
#     core.cpp 
#     cv.cpp
#     udp.cpp
# )

target_compile_features(main PUBLIC cxx_std_20)

# target_include_directories(main PRIVATE 
#     ${OpenCV_INCLUDE_DIRS} 
#     ${LIBCAMERA_INCLUDE_DIRS}
# )

# target_link_libraries(main PRIVATE 
#     ${OpenCV_LIBS} 
#     ${LIBCAMERA_LINK_LIBRARIES}
#     Qt5::Core 
#     Qt5::Network
# )
