#pragma once

#include <pixel_engine/entity.h>

namespace pxl {
class Scene {
 public:
  std::vector<std::shared_ptr<Entity>> entities;
};
}  // namespace pxl
