#include <pixel_engine/utilities.h>

namespace pxl {
Eigen::Matrix4f Orthagonal(float left, float right, float bottom, float top,
                           float near, float far) {
  Eigen::Matrix4f ortho = Eigen::Matrix4f::Zero();
  ortho(0, 0) = 2 / (right - left);
  ortho(1, 1) = 2 / (top - bottom);
  ortho(2, 2) = 2 / (near - far);
  ortho(0, 3) = -(right + left) / (right - left);
  ortho(1, 3) = -(top + bottom) / (top - bottom);
  ortho(2, 3) = -(far + near) / (far - near);
  return ortho;
}
}  // namespace pxl
