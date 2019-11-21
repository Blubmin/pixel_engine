#include <pixel_engine/physics_component.h>

#include <Eigen/Core>

namespace pxl {
PhysicsComponent::PhysicsComponent()
    : PhysicsComponent(Eigen::Vector3f::Zero()) {}

PhysicsComponent::PhysicsComponent(const Eigen::Vector3f& velocity)
    : velocity(velocity) {}
}  // namespace pxl