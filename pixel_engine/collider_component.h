#pragma once

#include <pixel_engine/component.h>

#include <memory>

#include <btBulletCollisionCommon.h>
#include <Eigen/Core>

#include <pixel_engine/mesh.h>

namespace pxl {
class ColliderComponent : public Component {
 public:
  enum Type { INVALID, kStatic, kDynamic };

  ColliderComponent(std::shared_ptr<btCollisionShape> shape,
                    ColliderComponent::Type collider_type,
                    Eigen::Vector3f offset = Eigen::Vector3f::Zero());

  btCollisionObject* GetCollisionObject() const;
  Type GetType() const;

 protected:
  std::shared_ptr<btCollisionShape> shape_;
  ColliderComponent::Type collider_type_;
  Eigen::Vector3f offset_;
};

class CapsuleCollider : public ColliderComponent {
 public:
  CapsuleCollider(float radius, float height,
                  ColliderComponent::Type collider_type);
};

class SphereCollider : public ColliderComponent {
 public:
  SphereCollider(float radius,
                  ColliderComponent::Type collider_type);
};

class BoxCollider : public ColliderComponent {
 public:
  BoxCollider(const Eigen::Vector3f& half_extents,
              ColliderComponent::Type collider_type);
};

class HullCollider : public ColliderComponent {
 public:
  HullCollider(const Mesh& mesh, ColliderComponent::Type collider_type);
};

class CombinedHullCollider : public ColliderComponent {
 public:
  CombinedHullCollider(const Mesh& mesh, ColliderComponent::Type collider_type);
};
}  // namespace pxl