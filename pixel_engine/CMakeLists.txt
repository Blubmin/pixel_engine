list(APPEND SOURCE_FILES
  camera.cpp
  color.cpp
  collider_component.cpp
  component.cpp
  convex_hull.cpp
  directional_light.cpp
  empty.cpp
  entity.cpp
  fps_controller.cpp
  free_camera_component.cpp
  game_mode.cpp
  game.cpp
  material.cpp
  mesh_loader.cpp
  mesh.cpp
  ogl_framebuffer.cpp
  ogl_fxaa_renderer.cpp
  ogl_gamma_renderer.cpp
  ogl_material.cpp
  ogl_mesh.cpp
  ogl_post_renderer.cpp
  ogl_ssao_renderer.cpp
  ogl_texture2d.cpp
  ogl_texture_renderer.cpp
  ogl_utilities.cpp
  physics_component.cpp
  program.cpp
  point_light.cpp
  scene.cpp
  scene_renderer.cpp
  skybox.cpp
  sphere.cpp
  texture2d.cpp
  thread_pool.cpp
  utilities.cpp
)

list(APPEND HEADER_FILES
  camera.h
  color.h
  collider_component.h
  component.h
  convex_hull.h
  directional_light.h
  eigen_utilities.h
  empty.h
  entity.h
  fps_controller.h
  free_camera_component.h
  game_mode.h
  game.h
  material.h
  mesh_loader.h
  mesh.h
  ogl_framebuffer.h
  ogl_fxaa_renderer.h
  ogl_gamma_renderer.h
  ogl_material.h
  ogl_mesh.h
  ogl_post_renderer.h
  ogl_ssao_renderer.h
  ogl_texture2d.h
  ogl_texture_renderer.h
  ogl_utilities.h
  physics_component.h
  program.h
  point_light.h
  scene.h
  scene_renderer.h
  skybox.h
  sphere.h
  texture2d.h
  thread_pool.h
  utilities.h
)

add_library(pixel_engine
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

target_link_libraries(pixel_engine
  ${Bullet}
  ${OPENGL_LIBRRIES}
  assimp
  Boost::filesystem
  gl3w
  glfw
  gflags
  glog
  imgui_glfw_opengl3
  OpenMP::OpenMP_CXX
)

set(PIXEL_ENGINE_HEADER_FILES "")
foreach(HDR_FILE ${HEADER_FILES})
  list(APPEND PIXEL_ENGINE_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HDR_FILE}")
endforeach(HDR_FILE)
set(PIXEL_ENGINE_HEADER_FILES "${PIXEL_ENGINE_HEADER_FILES}" PARENT_SCOPE)