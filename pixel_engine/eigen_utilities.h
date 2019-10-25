#pragma once

#include <Eigen/Geometry>

namespace Eigen {
typedef AlignedBox<float, 2> Rectf;

template <typename T>
using Vector2 = Matrix<T, 2, 1>;

template <typename T>
using Vector3 = Matrix<T, 3, 1>;

template <typename T>
using Vector4 = Matrix<T, 4, 1>;

template <typename T>
using Isometry3 = Transform<T, 3, Isometry>;

template <typename T>
using Matrix3 = Matrix<T, 3, 3>;

template <typename T>
using Matrix4 = Matrix<T, 4, 4>;

template <typename T>
Eigen::Vector3<T> GetPosition(const Eigen::Matrix4<T>& mat) {
  return mat.block<3, 1>(0, 3);
}

template <typename T>
Eigen::Matrix3<T> GetRotation(const Eigen::Matrix4<T>& mat) {
  return mat.block<3, 3>(0, 0);
}
}  // namespace Eigen