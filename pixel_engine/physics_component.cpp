#include <pixel_engine/physics_component.h>

#include <Eigen/Core>

namespace pxl {
PhysicsComponent::PhysicsComponent() : velocity(Eigen::Vector3f::Zero()) {}
}  // namespace pxl