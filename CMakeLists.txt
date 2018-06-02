cmake_minimum_required(VERSION 3.10)
project(PixelEngine
  VERSION 1.0.0
  LANGUAGES CXX
)

set(prefix ${CMAKE_PREFIX_PATH})
set(build_prefix ${CMAKE_PREFIX_PATH})

# OpenGL
find_package(OpenGL REQUIRED)

# Third Party Libraries
add_subdirectory(third_party)
include_directories(third_party/gl3w/include)
include_directories(third_party/glog/src)

# PixelEngine
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(pixel_engine)

# Examples
add_subdirectory(examples)