#pragma once

#include <memory>

namespace pxl {
class Entity;
class Component {
 public:
  virtual void Update(float time_elapsed) = 0;
  void SetOwner(std::shared_ptr<Entity> owner);

  std::weak_ptr<Entity> owner;
};
}  // namespace pxl
