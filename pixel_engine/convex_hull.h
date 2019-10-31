#pragma once

#include <pixel_engine/mesh.h>

namespace pxl {
class ConvexHull : public Mesh {
 public:
  ConvexHull();
  explicit ConvexHull(const Mesh* mesh);

 private:
  std::shared_ptr<SubMesh> QuickHull(const SubMesh& mesh);
};
}  // namespace pxl