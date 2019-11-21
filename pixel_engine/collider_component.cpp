#include <pixel_engine/collider_component.h>

#include <pixel_engine/convex_hull.h>
#include <pixel_engine/entity.h>

namespace pxl {
ColliderComponent::ColliderComponent(std::shared_ptr<btCollisionShape> shape,
                                     ColliderComponent::Type collider_type,
                                     Eigen::Vector3f offset)
    : shape_(shape), collider_type_(collider_type), offset_(offset) {}

btCollisionObject* ColliderComponent::GetCollisionObject() const {
  auto object = new btCollisionObject();
  object->setCollisionShape(shape_.get());
  object->getWorldTransform().setFromOpenGLMatrix(
      owner.lock()->GetTransform().data());
  object->getWorldTransform().setOrigin(
      object->getWorldTransform().getOrigin() +
      btVector3(offset_.x(), offset_.y(), offset_.z()));
  object->setUserPointer((void*)this);
  return object;
}

ColliderComponent::Type ColliderComponent::GetType() const {
  return collider_type_;
}

CapsuleCollider::CapsuleCollider(float radius, float height,
                                 ColliderComponent::Type collider_type)
    : ColliderComponent(std::shared_ptr<btCapsuleShape>(
                            new btCapsuleShape(radius, height - radius * 2)),
                        collider_type,
                        Eigen::Vector3f::UnitY() * (height / 2.f)) {}

SphereCollider::SphereCollider(float radius,
                               ColliderComponent::Type collider_type)
    : ColliderComponent(
          std::shared_ptr<btSphereShape>(new btSphereShape(radius)),
          collider_type) {}

BoxCollider::BoxCollider(const Eigen::Vector3f& half_extents,
                         ColliderComponent::Type collider_type)
    : ColliderComponent(
          std::shared_ptr<btBoxShape>(new btBoxShape(
              btVector3(half_extents.x(), half_extents.y(), half_extents.z()))),
          collider_type) {}

HullCollider::HullCollider(const Mesh& mesh,
                           ColliderComponent::Type collider_type)
    : ColliderComponent(std::shared_ptr<btCompoundShape>(new btCompoundShape()),
                        collider_type) {
  auto compound = std::static_pointer_cast<btCompoundShape>(shape_);
  auto hull = ConvexHull(&mesh);
  for (auto mesh : hull.sub_meshes) {
    auto bt_convex = new btConvexHullShape();
    for (size_t i = 0; i < mesh->positions.size(); i += 3) {
      bt_convex->addPoint(btVector3(mesh->positions[i], mesh->positions[i + 1],
                                    mesh->positions[i + 2]));
    }
    compound->addChildShape(btTransform::getIdentity(), bt_convex);
  }
}

CombinedHullCollider::CombinedHullCollider(
    const Mesh& mesh, ColliderComponent::Type collider_type)
    : ColliderComponent(
          std::shared_ptr<btConvexHullShape>(new btConvexHullShape()),
          collider_type) {
  auto bt_convex = std::static_pointer_cast<btConvexHullShape>(shape_);
  auto hull = ConvexHull(&mesh);
  for (auto mesh : hull.sub_meshes) {
    for (size_t i = 0; i < mesh->positions.size(); i += 3) {
      bt_convex->addPoint(btVector3(mesh->positions[i], mesh->positions[i + 1],
                                    mesh->positions[i + 2]));
    }
  }
  bt_convex->optimizeConvexHull();
}
}  // namespace pxl