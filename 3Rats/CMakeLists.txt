cmake_minimum_required(VERSION 3.10)
project(3Rats)

# Set C++ standard (optional)
set(CMAKE_CXX_STANDARD 17)

# Add all .cpp files in the src directory
file(GLOB SOURCES "src/*.cpp")

# Add all .h files in the src directory
file(GLOB HEADERS "src/*.h")

# Find SDL2, SDL2_mixer, and SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

# Add executable target
add_executable(3Rats ${SOURCES} ${HEADERS})

# Include SDL2 parent directory so <SDL2/SDL.h> resolves correctly
target_include_directories(3Rats PRIVATE /usr/local/include)

# Link SDL2, SDL2_mixer, SDL2_ttf, and SDL2_image libraries
target_link_libraries(3Rats PRIVATE SDL2::SDL2 SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
