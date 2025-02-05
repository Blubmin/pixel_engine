#include <pixel_engine/ogl_mesh.h>

#include <glog/logging.h>

#include <pixel_engine/program.h>

namespace pxl {
OglMesh::OglMesh() : draw_materials(true), is_bound_(false) {}

OglMesh::OglMesh(const Mesh& mesh) : OglMesh() {
  sub_meshes.resize(mesh.sub_meshes.size());
  std::copy(mesh.sub_meshes.begin(), mesh.sub_meshes.end(), sub_meshes.begin());
  materials.resize(mesh.materials.size());
  std::copy(mesh.materials.begin(), mesh.materials.end(), materials.begin());
}

OglMesh::OglMesh(std::shared_ptr<SubMesh> sub_mesh)
    : Mesh(sub_mesh), draw_materials(true), is_bound_(false) {}

OglMesh::~OglMesh() {
  if (!vaos.empty()) {
    glDeleteVertexArrays(vaos.size(), vaos.data());
    for (const auto buffer : buffers) {
      glDeleteBuffers(buffer.size(), buffer.data());
    }
  }
}

void OglMesh::Bind() {
  if (is_bound_) {
    return;
  }
  vaos.resize(sub_meshes.size(), -1);
  buffers.resize(sub_meshes.size(), std::vector<GLuint>(5));
  glGenVertexArrays(vaos.size(), vaos.data());
  for (size_t i = 0; i < sub_meshes.size(); ++i) {
    auto sub_mesh = sub_meshes[i];
    glBindVertexArray(vaos[i]);

    auto& buffer = buffers[i];
    glGenBuffers(buffer.size(), buffer.data());

    // Position
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sub_mesh->positions.size() * sizeof(GLfloat),
                 sub_mesh->positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Normal
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sub_mesh->normals.size() * sizeof(GLfloat),
                 sub_mesh->normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Tangent
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sub_mesh->tangents.size() * sizeof(GLfloat),
                 sub_mesh->tangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // UVs
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER,
                 sub_mesh->texture_coordinates.size() * sizeof(GLfloat),
                 sub_mesh->texture_coordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sub_mesh->triangles.size() * sizeof(GLuint),
                 sub_mesh->triangles.data(), GL_STATIC_DRAW);

    // Cleanup
    glBindVertexArray(0);
  }

  // Replace materials with Ogl counterpart
  for (auto& material : materials) {
    auto ogl_material = std::make_shared<OglMaterial>(*material);
    ogl_material->Bind();
    material = ogl_material;
  }

  is_bound_ = true;
}

void OglMesh::Draw(const Program& prog) {
  for (size_t i = 0; i < sub_meshes.size(); ++i) {
    const auto sub_mesh = sub_meshes[i];

    if (draw_materials) {
      const auto material = std::dynamic_pointer_cast<OglMaterial>(
          materials[sub_mesh->material_idx]);
      material->Use(prog);
    }

    glBindVertexArray(vaos[i]);
    glDrawElements(GL_TRIANGLES, sub_mesh->triangles.size(), GL_UNSIGNED_INT,
                   0);
  }
}
}  // namespace pxl
