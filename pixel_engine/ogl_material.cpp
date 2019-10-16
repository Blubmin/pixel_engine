#include <pixel_engine/ogl_material.h>

namespace pxl {

OglMaterial::OglMaterial() {}

OglMaterial::OglMaterial(const Material& material) : Material(material) {}

void OglMaterial::Bind() {
  // Load diffuse texture
  if (!diffuse_texture_path.empty()) {
    diffuse_texture = std::make_shared<OglTexture2d>(diffuse_texture_path);
    diffuse_texture->Bind();
    diffuse_texture->SetFilterMode(GL_LINEAR_MIPMAP_LINEAR);
    diffuse_texture->Free();
  } else {
    float data[3] = {0, 0, 0};
    diffuse_texture = std::make_shared<OglTexture2d>(data, 1, 1, 3);
    diffuse_texture->Bind();
  }

  // Load normal map
  if (!normal_map_path.empty()) {
    normal_map = std::make_shared<OglTexture2d>(normal_map_path);
    normal_map->Bind();
    normal_map->SetFilterMode(GL_LINEAR_MIPMAP_LINEAR);
    normal_map->Free();
  } else {
    float data[3] = {.5, .5, 1};
    normal_map = std::make_shared<OglTexture2d>(data, 1, 1, 3);
    normal_map->Bind();
  }
}

void OglMaterial::Use(const Program& prog, const std::string& ambient_name,
                      const std::string& diffuse_name,
                      const std::string& specular_name,
                      const std::string& shininess_name,
                      const std::string& diffuse_texture_name,
                      const std::string& normal_map_name) {
  prog.SetUniform3fv(ambient_name, ambient.data());
  prog.SetUniform3fv(diffuse_name, diffuse.data());
  prog.SetUniform3fv(specular_name, specular.data());
  prog.SetUniform1f(shininess_name, shininess);

  diffuse_texture->Use(0);
  prog.SetUniform1i(diffuse_texture_name, 0);
  normal_map->Use(1);
  prog.SetUniform1i(normal_map_name, 1);
}
}  // namespace pxl