cmake_minimum_required(VERSION 3.0.0...3.5.0)
project(fluid-sim VERSION 0.1.0)

# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2_cmake)

# Find the SDL2 library
find_package(SDL2 REQUIRED)

# ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

# SDL2_gfx
set(SDL2_GFX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_gfx)
include_directories(${SDL2_GFX_DIR})

# Specify the include directory for header files
include_directories(${SDL2_INCLUDE_DIRS} include)

# Add all source files from the 'src' directory
file(GLOB SOURCES "src/*.cpp")
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp)
file(GLOB SDL2_SOURCES ${SDL2_GFX_DIR}/*.c)
add_library(imgui STATIC ${IMGUI_SOURCES})
add_library(SDL2_gfx STATIC ${SDL2_SOURCES})

# Set the build type if it's not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optimizations and executable size reduction flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s") # -O3 for optimizations, -s for stripping

# Define your executable with the source files
add_executable(fluid-sim ${SOURCES})

# Link SDL2 to your executable
target_link_libraries(fluid-sim imgui ${SDL2_LIBRARIES} SDL2_gfx)
