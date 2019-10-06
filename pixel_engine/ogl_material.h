#pragma once

#include <pixel_engine/material.h>

#include <memory>

#include <pixel_engine/ogl_texture2d.h>
#include <pixel_engine/program.h>

namespace pxl {

class OglMaterial : public Material {
 public:
  OglMaterial();
  explicit OglMaterial(const Material& material);
  ~OglMaterial() override {}

  void Bind();
  void Use(
      const Program& prog,
      const std::string& ambient_name = "u_material.ambient",
      const std::string& diffuse_name = "u_material.diffuse",
      const std::string& specular_name = "u_material.specular",
      const std::string& shininess_name = "u_material.shininess",
      const std::string& diffuse_texture_name = "u_material.diffuse_texture");

 public:
  std::shared_ptr<OglTexture2d> diffuse_texture;
};

}  // namespace pxl