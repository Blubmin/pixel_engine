# GLFW3
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# GL3W
add_subdirectory(gl3w)

# gflags
set(GFLAGS_BUILD_TESTING OFF)
set(GFLAGS_BUILD_gflags_nothreads_LIB OFF)
set(GFLAGS_BUILD_gflags_LIB ON)
add_subdirectory(gflags)

# glog
set(WITH_GFLAGS OFF)
set(BUILD_TESTING OFF)
add_subdirectory(glog)

# Assimp
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)

# ImGui
add_subdirectory(imgui)