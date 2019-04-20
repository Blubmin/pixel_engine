#pragma once

#include <Eigen/Core>

namespace pxl {
class Entity {
 public:
  Entity();

  Eigen::Vector3f position;
  Eigen::Vector3f rotation;
  Eigen::Vector3f scale;

  Eigen::Matrix4f GetTransform();
};
}  // namespace pxl
