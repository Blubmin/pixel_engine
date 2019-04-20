cmake_minimum_required(VERSION 3.12)
project(PixelEngine
  VERSION 1.0.0
)
set(CMAKE_CXX_STANDARD 20)

set(prefix ${CMAKE_PREFIX_PATH})
set(build_prefix ${CMAKE_PREFIX_PATH})

# OpenGL
find_package(OpenGL REQUIRED)

# Boost
find_package(Boost REQUIRED filesystem)

# Third Party Libraries
include_directories(third_party)
include_directories(third_party/assimp/include)
include_directories(third_party/eigen)
include_directories(third_party/gl3w/include)
include_directories(third_party/glog/src)
include_directories(third_party/gflags/src)
add_subdirectory(third_party)

# PixelEngine
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(pixel_engine)

# Examples
add_subdirectory(examples)