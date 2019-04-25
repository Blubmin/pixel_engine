#include <pixel_engine/entity.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Eigen/Geometry>

namespace pxl {
Entity::Entity(std::shared_ptr<Entity> parent)
    : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), parent(parent) {}

void Entity::Update(float time_elapsed) {
  for (auto component : components) {
    component->Update(time_elapsed);
  }
  for (auto child : children) {
    child->Update(time_elapsed);
  }
}

void Entity::SetParent(std::shared_ptr<Entity> parent) {
  this->parent = parent;
}

void Entity::AddChild(std::shared_ptr<Entity> child) {
  if (child == nullptr) {
    return;
  }
  children.push_back(child);
  child->SetParent(shared_from_this());
}

void Entity::AddComponent(std::shared_ptr<Component> component) {
  if (component == nullptr) {
    return;
  }
  components.push_back(component);
  component->SetOwner(shared_from_this());
}

Eigen::Matrix4f Entity::GetTransform() {
  Eigen::Affine3f transform = Eigen::Affine3f::Identity();
  if (!parent.expired()) {
    auto p = parent.lock();
    transform = p->GetTransform();
  }
  transform =
      transform * Eigen::Translation3f(position) *
      Eigen::AngleAxisf(rotation.y() * M_PI / 180, Eigen::Vector3f::UnitY()) *
      Eigen::AngleAxisf(rotation.x() * M_PI / 180, Eigen::Vector3f::UnitX()) *
      Eigen::AngleAxisf(rotation.z() * M_PI / 180, Eigen::Vector3f::UnitZ()) *
      Eigen::Scaling(scale);
  return transform.matrix();
}
}  // namespace pxl
