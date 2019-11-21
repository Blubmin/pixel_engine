#pragma once

#include <pixel_engine/component.h>

#include <Eigen/Core>

namespace pxl {
class PhysicsComponent : public Component {
 public:
  PhysicsComponent();
  PhysicsComponent(const Eigen::Vector3f& velocity);

  Eigen::Vector3f velocity;
};
}  // namespace pxl