#pragma once

#include <Eigen/Core>

#include <pixel_engine/entity.h>
#include <pixel_engine/mesh.h>
#include <pixel_engine/program.h>

namespace pxl {
class Camera : public Entity {
 public:
  Camera();
  void Draw(const Program& prog);

  float fov;
  float near_plane;
  float far_plane;

  Eigen::Matrix4f GetPerspective() const;
  Eigen::Matrix4f GetView() const;

 private:
  static std::shared_ptr<MeshEntity> mesh_;
};
}  // namespace pxl
