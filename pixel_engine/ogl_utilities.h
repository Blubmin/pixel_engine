#pragma once

#include <string>

#include <GL/gl3w.h>>
#include <glog/logging.h>
#include <Eigen/Core>

#define OGL_LOG_ERROR(severity)                                       \
  {                                                                   \
    GLenum err = glGetError();                                        \
    LOG_IF(severity, err != GL_NO_ERROR) << pxl::GetErrorString(err); \
  }

namespace pxl {
std::string GetErrorString(GLenum err);

Eigen::Matrix4f GetOrthoProjection(float left, float right, float bottom,
                                   float top, float far_plane,
                                   float near_plane);
}  // namespace pxl
