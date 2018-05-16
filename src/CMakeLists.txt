list(APPEND SOURCE_FILES
  game.cpp
  program.cpp
)

add_library(pixel_engine
  ${SOURCE_FILES}
)

target_link_libraries(pixel_engine
  ${OpenGL_LIBRRIES}
  glfw
)

set(PIXEL_ENGINE_SRC_FILES ${SOURCE_FILES} PARENT_SCOPE)