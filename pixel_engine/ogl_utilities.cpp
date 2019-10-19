#include <pixel_engine/ogl_utilities.h>

namespace pxl {
std::string GetErrorString(GLenum err) {
  switch (err) {
    case GL_NO_ERROR:
      return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
    default:
      return "UNKNOWN_ERROR";
  }
}

Eigen::Matrix4f GetOrthoProjection(float left, float right, float bottom,
                                   float top, float far_plane,
                                   float near_plane) {
  Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
  mat(0, 0) = 2 / (right - left);
  mat(1, 1) = 2 / (top - bottom);
  mat(2, 2) = -2 / (far_plane - near_plane);
  mat(0, 3) = -(right + left) / (right - left);
  mat(1, 3) = -(top + bottom) / (top - bottom);
  mat(2, 3) = -(far_plane + near_plane) / (far_plane - near_plane);
  return mat;
}
}  // namespace pxl
