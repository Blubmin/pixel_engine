#include <pixel_engine/component.h>

namespace pxl {
void Component::SetOwner(std::shared_ptr<Entity> owner) {
  if (owner == nullptr) {
    return;
  }
  this->owner = owner;
}
}  // namespace pxl
