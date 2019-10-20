#pragma once

#include <pixel_engine/component.h>

#include <Eigen/Core>

namespace pxl {
class FpsController : public Component {
 public:
  FpsController();

  void Update(float time_elapsed) override;

  float speed;

 private:
  Eigen::Vector3f GetMovementVector() const;
  void Rotate();
};
}  // namespace pxl