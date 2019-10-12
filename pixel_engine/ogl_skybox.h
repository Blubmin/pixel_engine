#pragma once

#include <pixel_engine/skybox.h>

namespace pxl {
class OglSkybox public : Skybox {
 public:
  Skybox();

  void Draw() override;
};
}  // namespace pxl