#include <pixel_engine/ogl_material.h>

namespace pxl {

OglMaterial::OglMaterial() {}

OglMaterial::OglMaterial(const Material& material) : Material(material) {}

void OglMaterial::Bind() {
  if (!diffuse_texture_path.empty()) {
    diffuse_texture = std::make_shared<OglTexture2d>(diffuse_texture_path);
    diffuse_texture->Bind();
    diffuse_texture->Free();
  } else {
    float data[3] = {0, 0, 0};
    diffuse_texture = std::make_shared<OglTexture2d>(data, 1, 1, 3);
    diffuse_texture->Bind();
  }
}

void OglMaterial::Use(const Program& prog, const std::string& ambient_name,
                      const std::string& diffuse_name,
                      const std::string& specular_name,
                      const std::string& shininess_name,
                      const std::string& diffuse_texture_name) {
  prog.SetUniform3fv(ambient_name, ambient.data());
  prog.SetUniform3fv(diffuse_name, diffuse.data());
  prog.SetUniform3fv(specular_name, specular.data());
  prog.SetUniform1f(shininess_name, shininess);

  diffuse_texture->Use(0);
  prog.SetUniform1i(diffuse_texture_name, 0);
}
}  // namespace pxl