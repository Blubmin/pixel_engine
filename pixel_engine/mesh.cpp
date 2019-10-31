#include <pixel_engine/mesh.h>

#include <pixel_engine/ogl_mesh.h>

namespace pxl {
void SubMesh::ComputeNormalsAndTangents() {}

void SubMesh::ComputeTangents() {}

Mesh::Mesh(std::shared_ptr<SubMesh> sub_mesh) : sub_meshes() {
  sub_meshes.push_back(sub_mesh);
}

MeshEntity::MeshEntity(std::shared_ptr<Mesh> mesh) : mesh(mesh) {}

void MeshEntity::Bind() {
  AddComponent(mesh);
  mesh->Bind();
}

void MeshEntity::Draw(const Program& prog) {
  auto mesh_components = GetComponents<OglMesh>();
  for (auto mesh_component : mesh_components) {
    mesh_component->Draw(prog);
  }
}
}  // namespace pxl
