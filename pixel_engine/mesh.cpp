#include <pixel_engine/mesh.h>

namespace pxl {
Mesh::Mesh(std::shared_ptr<SubMesh> sub_mesh) : sub_meshes() {
  sub_meshes.push_back(sub_mesh);
}

MeshEntity::MeshEntity(std::shared_ptr<Mesh> mesh) : mesh(mesh) {}

void MeshEntity::Bind() {
  AddComponent(mesh);
  mesh->Bind();
}

void MeshEntity::Draw(const Program& prog) { mesh->Draw(prog); }
}  // namespace pxl
