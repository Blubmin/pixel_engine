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
  perspective(0, 0) = 1 / (half_tan * Game::State.GetAspectRatio());
  perspective(1, 1) = 1 / half_tan;
  perspective(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
  perspective(2, 3) = (2 * far_plane * near_plane) / (near_plane - far_plane);
  perspective(3, 2) = -1;
  perspective(3, 3) = 1;
  return perspective;
}

Eigen::Matrix4f Camera::GetView() {
  Eigen::Isometry3f transform;
  transform.translation() = position;
  transform.linear() =
      (Eigen::AngleAxisf(rotation.y(), Eigen::Vector3f::UnitY()) *
       Eigen::AngleAxisf(rotation.x(), Eigen::Vector3f::UnitX()) *
       Eigen::AngleAxisf(rotation.z(), Eigen::Vector3f::UnitZ()))
          .toRotationMatrix();
  return transform.inverse().matrix();
}
}  // namespace pxl
