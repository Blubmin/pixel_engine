#pragma once

#include <vector>

#include <Eigen/Core>
#include <boost/optional.hpp>

namespace pxl {
class Entity : public std::enable_shared_from_this<Entity> {
 public:
  Entity(std::shared_ptr<Entity> parent = nullptr);
  virtual ~Entity(){};

  void SetParent(std::shared_ptr<Entity> parent);
  void AddChild(std::shared_ptr<Entity> child);
  Eigen::Matrix4f GetTransform();

  std::weak_ptr<Entity> parent;
  std::vector<std::shared_ptr<Entity>> children;

  Eigen::Vector3f position;
  Eigen::Vector3f rotation;
  Eigen::Vector3f scale;
};
}  // namespace pxl
