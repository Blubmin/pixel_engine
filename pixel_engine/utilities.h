#pragma once

#include <Eigen/Core>

namespace pxl {
Eigen::Matrix4f Orthagonal(float left, float right, float bottom, float top,
                           float near, float far);
}
