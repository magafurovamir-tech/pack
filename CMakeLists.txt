cmake_minimum_required(VERSION 3.10)
project(PackUtility C)

# Set the output directory for the executable to /build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Add compiler flags for warnings and optimization
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

add_executable(pack main.c)