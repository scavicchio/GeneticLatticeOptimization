cmake_minimum_required(VERSION 3.5)
project(TPA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

MESSAGE( STATUS "Starting build process")

add_subdirectory(lib/Titan)
add_subdirectory(lib/lib3mf)
add_subdirectory(lib/spectra)

include_directories(
        lib/Titan/include
        lib/lib3mf/Include
        lib/spectra/include
        )

message(WARNING ${COMPILE_DEFINITIONS})

set(SOURCES
		src/main.cpp
)

include_directories(src)

