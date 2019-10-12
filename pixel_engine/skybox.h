#pragma once

#include <pixel_engine/entity.h>
#include <pixel_engine/mesh.h>

namespace pxl {

class Skybox : public Entity {
 public:
  Skybox(std::shared_ptr<Mesh> mesh);

  std::shared_ptr<Mesh> mesh;
};
}  // namespace pxl