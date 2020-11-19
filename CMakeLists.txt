cmake_minimum_required(VERSION 3.5)
project(TPA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

MESSAGE( STATUS "Starting build process")

add_subdirectory(lib/Titan)
add_subdirectory(lib/lib3mf)

include_directories(
        lib/Titan/include
        lib/lib3mf/Include
        )

message(WARNING ${COMPILE_DEFINITIONS})

set(SOURCES
		src/main.cpp
)

include_directories(src)

# Enable OpenMP compiler options if available
option (USE_OpenMP "Use OpenMP" ON)
if (USE_OpenMP)
        find_package(OpenMP)
        if (OpenMP_CXX_FOUND)
                SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        endif()
endif()


