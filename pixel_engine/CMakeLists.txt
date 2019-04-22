list(APPEND SOURCE_FILES
  camera.cpp
  color.cpp
  component.cpp
  entity.cpp
  game_mode.cpp
  game.cpp
  mesh_loader.cpp
  mesh.cpp
  ogl_framebuffer.cpp
  ogl_fxaa_renderer.cpp
  ogl_mesh.cpp
  ogl_texture2d.cpp
  ogl_texture_renderer.cpp
  ogl_utilities.cpp
  program.cpp
  point_light.cpp
  texture2d.cpp
)

list(APPEND HEADER_FILES
  camera.h
  color.h
  component.h
  entity.h
  game_mode.h
  game.h
  mesh_loader.h
  mesh.h
  ogl_framebuffer.h
  ogl_fxaa_renderer.h
  ogl_mesh.h
  ogl_texture2d.h
  ogl_texture_renderer.h
  ogl_utilities.h
  program.h
  point_light.h
  texture2d.h
)

add_library(pixel_engine
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

target_include_directories(pixel_engine PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gl3w/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include
)

target_link_libraries(pixel_engine
  ${OPENGL_LIBRRIES}
  assimp
  Boost::filesystem
  gl3w
  glfw
  gflags
  glog
)

set(PIXEL_ENGINE_HEADER_FILES "")
foreach(HDR_FILE ${HEADER_FILES})
  list(APPEND PIXEL_ENGINE_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HDR_FILE}")
endforeach(HDR_FILE)
set(PIXEL_ENGINE_HEADER_FILES "${PIXEL_ENGINE_HEADER_FILES}" PARENT_SCOPE)