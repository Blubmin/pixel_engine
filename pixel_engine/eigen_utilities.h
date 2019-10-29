#pragma once

#include <Eigen/Geometry>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

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
Eigen::Vector3<T> GetXAxis(const Eigen::Matrix4<T>& mat) {
  return mat.block<3, 1>(0, 0);
}

template <typename T>
Eigen::Vector3<T> GetYAxis(const Eigen::Matrix4<T>& mat) {
  return mat.block<3, 1>(0, 1);
}

template <typename T>
Eigen::Vector3<T> GetZAxis(const Eigen::Matrix4<T>& mat) {
  return mat.block<3, 1>(0, 2);
}

template <typename T>
Eigen::Vector3<T> GetPosition(const Eigen::Matrix4<T>& mat) {
  return mat.block<3, 1>(0, 3);
}

template <typename T>
Eigen::Matrix3<T> GetRotation(const Eigen::Matrix4<T>& mat) {
  return mat.block<3, 3>(0, 0);
}

template <typename T>
Eigen::Vector3<T> Min(const Eigen::Vector3<T>& a, const Eigen::Vector3<T>& b) {
  return Eigen::Vector3f(MIN(a.x(), b.x()), MIN(a.y(), b.y()),
                         MIN(a.z(), b.z()));
}

template <typename T>
Eigen::Vector3<T> Max(const Eigen::Vector3<T>& a, const Eigen::Vector3<T>& b) {
  return Eigen::Vector3f(MAX(a.x(), b.x()), MAX(a.y(), b.y()),
                         MAX(a.z(), b.z()));
}
}  // namespace Eigen