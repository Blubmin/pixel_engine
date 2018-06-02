list(APPEND SOURCE_FILES
  game.cpp
  program.cpp
)

list(APPEND HEADER_FILES
  game.h
  program.h
)

add_library(pixel_engine
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

target_include_directories(pixel_engine PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gl3w/include
)

message(STATUS "gflag_LIBRARIES=${gflag_LIBRARIES}")

target_link_libraries(pixel_engine
  ${OPENGL_LIBRRIES}
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