#pragma once

#include <memory>

#include <pixel_engine/program.h>

namespace pxl {
class Entity;
class Component {
 public:
  virtual void Update(float time_elapsed) {}
  virtual void Draw(const Program& prog) {}
  void SetOwner(std::shared_ptr<Entity> owner);
  std::shared_ptr<Entity> GetOwner() const;

  std::weak_ptr<Entity> owner;
};
}  // namespace pxl
