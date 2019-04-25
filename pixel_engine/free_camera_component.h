#pragma once

#include <pixel_engine/component.h>

namespace pxl {
class FreeCameraComponent : public Component {
 public:
  void Update(float time_elapsed);
};
}  // namespace pxl
