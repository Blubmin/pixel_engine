#pragma once

#include <string>

#include <GL/gl3w.h>>
#include <glog/logging.h>

#define OGL_LOG_ERROR(severity)                                       \
  {                                                                   \
    GLenum err = glGetError();                                        \
    LOG_IF(severity, err != GL_NO_ERROR) << pxl::GetErrorString(err); \
  }

namespace pxl {
std::string GetErrorString(GLenum err);
}  // namespace pxl
