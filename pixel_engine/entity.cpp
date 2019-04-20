#include <pixel_engine/entity.h>

#include <Eigen/Geometry>

namespace pxl {
Entity::Entity() : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1) {}

Eigen::Matrix4f Entity::GetTransform() {
  Eigen::Affine3f transform;
  transform.translation() = position;
  transform.linear() =
      (Eigen::AngleAxisf(rotation.y(), Eigen::Vector3f::UnitY()) *
       Eigen::AngleAxisf(rotation.x(), Eigen::Vector3f::UnitX()) *
       Eigen::AngleAxisf(rotation.z(), Eigen::Vector3f::UnitZ()))
          .toRotationMatrix();
  transform.scale(scale);
  return transform.matrix();
}
}  // namespace pxl
