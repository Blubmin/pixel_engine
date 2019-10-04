#pragma once

#include <pixel_engine/texture2d.h>

#include <GL/gl3w.h>>

namespace pxl {
class OglTexture2d : public Texture2d {
 public:
  OglTexture2d(uint32_t width, uint32_t height, Format format);
  ~OglTexture2d() override;

  void Bind() override;

  GLuint GetTextureId() const;
  void Use(uint32_t texture_unit) const;

 private:
  GLuint texture_id;
};
}  // namespace pxl
