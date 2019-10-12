#include <pixel_engine/directional_light.h>

namespace pxl {
DirectionalLight::DirectionalLight(const Eigen::Vector3f& direction,
                                   float strength, const Eigen::Vector3f& color)
    : direction(direction), strength(strength), color(color) {}
}  // namespace pxl