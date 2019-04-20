#include <pixel_engine/camera.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Eigen/Geometry>

#include <pixel_engine/game.h>

namespace pxl {
Camera::Camera() : fov(90), near_plane(0.01), far_plane(1000) {}

Eigen::Matrix4f Camera::GetPerspective() {
  Eigen::Matrix4f perspective = Eigen::Matrix4f::Zero();
  float t =
      1 / (std::tan(fov / 2.f * M_PI / 180.f) * Game::State.GetAspectRatio());
  float s = 1 / (std::tan(fov / 2.f * M_PI / 180.f));
  perspective(0, 0) = t;
  perspective(1, 1) = s;
  perspective(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
  perspective(2, 3) = (2 * far_plane * near_plane) / (near_plane - far_plane);
  perspective(3, 2) = -1;
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
