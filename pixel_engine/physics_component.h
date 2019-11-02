#pragma once

#include <pixel_engine/component.h>

#include <Eigen/Core>

namespace pxl {
class PhysicsComponent : public Component {
 public:
  PhysicsComponent();

  Eigen::Vector3f velocity;
};
}  // namespace pxl