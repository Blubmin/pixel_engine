#include <pixel_engine/point_light.h>

namespace pxl {
PointLight::PointLight(const Color& color, float linear_attenuation,
                       float quadratic_attenuation)
    : color(color),
      linear_attenuation(linear_attenuation),
      quadratic_attenuation(quadratic_attenuation) {}
}  // namespace pxl
