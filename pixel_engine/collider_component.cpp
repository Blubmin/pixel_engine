#include <pixel_engine/collider_component.h>

#include <pixel_engine/entity.h>

namespace pxl {
ColliderComponent::ColliderComponent(std::shared_ptr<btCollisionShape> shape,
                                     ColliderComponent::Type collider_type)
    : shape_(shape), collider_type_(collider_type) {}

btCollisionObject* ColliderComponent::GetCollisionObject() const {
  auto object = new btCollisionObject();
  object->setCollisionShape(shape_.get());
  object->getWorldTransform().setFromOpenGLMatrix(
      owner.lock()->GetTransform().data());
  object->setUserPointer((void*)this);
  return object;
}

ColliderComponent::Type ColliderComponent::GetType() const {
  return collider_type_;
}

CapsuleCollider::CapsuleCollider(float radius, float height,
                                 ColliderComponent::Type collider_type)
    : ColliderComponent(
          std::shared_ptr<btCapsuleShape>(new btCapsuleShape(radius, height)),
          collider_type) {}

BoxCollider::BoxCollider(const Eigen::Vector3f& half_extents,
                         ColliderComponent::Type collider_type)
    : ColliderComponent(
          std::shared_ptr<btBoxShape>(new btBoxShape(
              btVector3(half_extents.x(), half_extents.y(), half_extents.z()))),
          collider_type) {}
}  // namespace pxl