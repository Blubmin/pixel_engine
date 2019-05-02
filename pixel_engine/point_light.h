#pragma once

#include <pixel_engine/color.h>
#include <pixel_engine/entity.h>

namespace pxl {
class PointLight : public Entity {
 public:
  PointLight(const Color& color, float linear_attenuation = .7,
             float quadratic_attenuation = 1.8);
  Color color;
  float linear_attenuation;
  float quadratic_attenuation;
};
}  // namespace pxl
