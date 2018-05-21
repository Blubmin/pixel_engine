cmake_minimum_required(VERSION 3.10)
project(PixelEngine
  VERSION 1.0.0
  LANGUAGES CXX
)

# OpenGL
find_package(OpenGL REQUIRED)

# Third Party Libraries
add_subdirectory(third_party)
include_directories(third_party/gl3w/include)

# PixelEngine
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(pixel_engine)

# Examples
add_subdirectory(examples)