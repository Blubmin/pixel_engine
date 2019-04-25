#pragma once

#include <vector>

#include <Eigen/Core>
#include <boost/optional.hpp>

#include <pixel_engine/component.h>

namespace pxl {
class Entity : public std::enable_shared_from_this<Entity> {
 public:
  Entity(std::shared_ptr<Entity> parent = nullptr);
  virtual ~Entity(){};

  void Update(float time_elapsed);

  void SetParent(std::shared_ptr<Entity> parent);
  void AddChild(std::shared_ptr<Entity> child);
  void AddComponent(std::shared_ptr<Component> component);
  Eigen::Matrix4f GetTransform();

  std::weak_ptr<Entity> parent;
  std::vector<std::shared_ptr<Entity>> children;
  std::vector<std::shared_ptr<Component>> components;

  Eigen::Vector3f position;
  Eigen::Vector3f rotation;
  Eigen::Vector3f scale;
};
}  // namespace pxl
