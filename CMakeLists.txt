cmake_minimum_required(VERSION 3.13)  # CMake version check
project(headercompression)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard
set(CMAKE_BUILD_TYPE Debug)
# Add main.cpp file of the project root directory as a source file
set(SOURCE_FILES main.cpp)
find_package(Threads REQUIRED)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(headercompression ${SOURCE_FILES})

target_link_libraries(headercompression explain Threads::Threads)