#include <pixel_engine/skybox.h>

#include <pixel_engine/ogl_material.h>

namespace pxl {
Skybox::Skybox(std::shared_ptr<Mesh> mesh) : mesh(mesh) {
  for (auto material : mesh->materials) {
    std::dynamic_pointer_cast<OglMaterial>(material)
        ->diffuse_texture->SetWrapMode(GL_CLAMP_TO_EDGE);
  }
}
}  // namespace pxl