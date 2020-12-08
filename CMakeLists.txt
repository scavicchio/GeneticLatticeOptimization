cmake_minimum_required(VERSION 3.5.0)
set(PROJECT_NAME "ANALYSIS")
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX CUDA) 

set(CMAKE_INCLUDE_CURRENT_DIR ON)
MESSAGE( STATUS "Starting build process")

add_subdirectory(lib/lib3mf)
add_subdirectory(lib/Titan)

include_directories(
        lib/Titan/include
        Inc/3mf
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

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME}
	PRIVATE Titan
	PRIVATE 3mf
)
