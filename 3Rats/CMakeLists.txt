cmake_minimum_required(VERSION 3.14)
project(3Rats)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use old C++11 ABI for GCC compatibility with SDL2 libraries
# Note: This is required because SDL2 libs on macOS are built with old ABI
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

# Option to build tests
option(BUILD_TESTS "Build test suite" ON)

# Find SDL2 libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

# Create library from source files (excludes main.cpp for reuse in tests)
file(GLOB LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
file(GLOB HEADERS "src/*.h")

add_library(3RatsLib STATIC ${LIB_SOURCES} ${HEADERS})
target_include_directories(3RatsLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    /usr/local/include
)
target_link_libraries(3RatsLib PUBLIC
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
)

# Main executable
add_executable(3Rats src/main.cpp)
target_link_libraries(3Rats PRIVATE 3RatsLib)

# Simple test framework integration
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
