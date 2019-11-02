cmake_minimum_required(VERSION 3.12)
project(PixelEngine
  VERSION 1.0.0
)
set(CMAKE_CXX_STANDARD 20)

set(prefix ${CMAKE_PREFIX_PATH})
set(build_prefix ${CMAKE_PREFIX_PATH})

# OpenGL
find_package(OpenGL REQUIRED)

# OpenMP
find_package(OpenMP REQUIRED)

# Boost
find_package(Boost REQUIRED filesystem)

# Bullet
# find_package(Bullet REQUIRED)
# include_directories(${BULLET_INCLUDE_DIRS})

# Third Party Libraries
add_definitions(-DImDrawIdx=uint32_t)
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)
add_definitions(-D_HAS_TR1_NAMESPACE)
include_directories(third_party)
include_directories(third_party/assimp/include)
include_directories(third_party/eigen)
include_directories(third_party/gl3w/include)
include_directories(third_party/glog/src)
include_directories(third_party/gflags/src)
add_subdirectory(third_party)

# Bullet
set(Bullet
	Bullet3Collision
	Bullet3Common
	Bullet3Dynamics
	Bullet3Geometry
	BulletCollision
	BulletDynamics
	BulletSoftBody
	LinearMath
)

# PixelEngine
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(pixel_engine)

# Examples
add_subdirectory(examples)