#pragma once

#include <pixel_engine/texture2d.h>

#include <GL/gl3w.h>>
#include <boost/filesystem.hpp>

namespace pxl {
class OglTexture2d : public Texture2d {
 public:
  OglTexture2d(int32_t width, int32_t height, Format format);
  OglTexture2d(float* data, int32_t width, int32_t height, int32_t channels);
  OglTexture2d(const boost::filesystem::path& path);
  ~OglTexture2d() override;

  void Bind() override;

  GLuint GetTextureId() const;
  void Use(uint32_t texture_unit) const;

 private:
  GLuint texture_id;
};
}  // namespace pxl
