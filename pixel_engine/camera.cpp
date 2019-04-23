#include <pixel_engine/camera.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Eigen/Geometry>

#include <pixel_engine/game.h>

namespace pxl {
Camera::Camera() : fov(90.f), near_plane(0.01f), far_plane(1000.f) {}

Eigen::Matrix4f Camera::GetPerspective() {
  Eigen::Matrix4f perspective = Eigen::Matrix4f::Zero();
  float half_tan = std::tan(fov / 2.f * M_PI / 180.f);
  perspective(0, 0) = 1.f / half_tan;
  perspective(1, 1) = 1.f / (half_tan / Game::State.GetAspectRatio());
  perspective(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
  perspective(2, 3) = (2.f * far_plane * near_plane) / (near_plane - far_plane);
  perspective(3, 2) = -1;
  return perspective;
}

Eigen::Matrix4f Camera::GetView() {
  Eigen::Affine3f transform(GetTransform());
  Eigen::Isometry3f isometry;
  isometry.translation() = transform.translation();
  isometry.linear() = transform.rotation();
  return isometry.inverse().matrix();
}
}  // namespace pxl
