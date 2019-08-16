#pragma once

#include <Eigen/Core>

#include <pixel_engine/entity.h>
#include <pixel_engine/mesh.h>

namespace pxl {
class Camera : public Entity {
 public:
  Camera();
  void Draw();

  float fov;
  float near_plane;
  float far_plane;

  Eigen::Matrix4f GetPerspective();
  Eigen::Matrix4f GetView();

 private:
  static std::shared_ptr<Mesh> mesh_;
};
}  // namespace pxl
