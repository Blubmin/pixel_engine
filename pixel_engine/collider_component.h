#pragma once

#include <pixel_engine/component.h>

#include <memory>

#include <btBulletCollisionCommon.h>
#include <Eigen/Core>

namespace pxl {
class ColliderComponent : public Component {
 public:
  enum Type { INVALID, kStatic, kDynamic };

  ColliderComponent(std::shared_ptr<btCollisionShape> shape,
                    ColliderComponent::Type collider_type);

  btCollisionObject* GetCollisionObject() const;
  Type GetType() const;

 private:
  std::shared_ptr<btCollisionShape> shape_;
  ColliderComponent::Type collider_type_;
};

class CapsuleCollider : public ColliderComponent {
 public:
  CapsuleCollider(float radius, float height,
                  ColliderComponent::Type collider_type);
};

class BoxCollider : public ColliderComponent {
 public:
  BoxCollider(const Eigen::Vector3f& half_extents,
              ColliderComponent::Type collider_type);
};
}  // namespace pxl