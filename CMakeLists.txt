cmake_minimum_required(VERSION 3.10)
project(PixelEngine
  VERSION 1.0.0
  LANGUAGES CXX
)

# OpenGL
find_package(OpenGL REQUIRED)

# Third Party Libraries
add_subdirectory(third_party)

# PixelEngines
include_directories(include)
add_subdirectory(src)

# Examples
add_subdirectory(examples)