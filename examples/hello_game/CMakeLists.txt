add_executable(hello_game
  hello_game.cpp
)

target_link_libraries(hello_game
  pixel_engine
  ${OPENGL_LIBRRIES}
  gl3w
  glfw
  gflags
  glog
)