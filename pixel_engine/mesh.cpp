#include <pixel_engine/mesh.h>

namespace pxl {
Mesh::Mesh(std::shared_ptr<SubMesh> sub_mesh) : sub_meshes() {
  sub_meshes.push_back(sub_mesh);
}
}  // namespace pxl
