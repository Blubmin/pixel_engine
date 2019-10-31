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

  void AddChild(std::shared_ptr<Entity> child);
  void AddComponent(std::shared_ptr<Component> component);

  template <typename ComponentType>
  std::shared_ptr<ComponentType> GetComponent() {
    for (auto component : components) {
      auto typed_component =
          std::dynamic_pointer_cast<ComponentType, Component>(component);
      if (typed_component != nullptr) {
        return typed_component;
      }
    }
    return nullptr;
  }

  template <typename ComponentType>
  std::vector<std::shared_ptr<ComponentType>> GetComponents() {
    std::vector<std::shared_ptr<ComponentType>> typed_components;
    for (auto component : components) {
      auto typed_component =
          std::dynamic_pointer_cast<ComponentType, Component>(component);
      if (typed_component != nullptr) {
        typed_components.push_back(typed_component);
      }
    }
    return typed_components;
  }

  template <typename ChildType>
  std::shared_ptr<ChildType> GetChild() {
    for (auto child : children) {
      auto typed_child = std::dynamic_pointer_cast<ChildType>(child);
      if (typed_child != nullptr) {
        return typed_child;
      }
    }
    return nullptr;
  }

  Eigen::Matrix4f GetTransform() const;

  std::weak_ptr<Entity> parent;
  std::vector<std::shared_ptr<Entity>> children;
  std::vector<std::shared_ptr<Component>> components;

  Eigen::Vector3f position;
  Eigen::Vector3f rotation;
  Eigen::Vector3f scale;

 protected:
  void SetParent(std::shared_ptr<Entity> parent);
};
}  // namespace pxl
