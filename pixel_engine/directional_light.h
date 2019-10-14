#pragma once

#include <pixel_engine/entity.h>

namespace pxl {
class DirectionalLight : public Entity {
 public:
  DirectionalLight(const Eigen::Vector3f& direction, float strength = 1.0,
                   const Eigen::Vector3f& color = Eigen::Vector3f::Ones());

  Eigen::Vector3f direction;
  float strength;
  Eigen::Vector3f color;
};
}  // namespace pxl