#pragma once

#include <Eigen/Core>

#include <pixel_engine/entity.h>

namespace pxl {
class Camera : public Entity {
 public:
  Camera();

  float fov;
  float near_plane;
  float far_plane;

  Eigen::Matrix4f GetPerspective();
  Eigen::Matrix4f GetView();
};
}  // namespace pxl
