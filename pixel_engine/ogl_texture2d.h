#pragma once

#include <pixel_engine/texture2d.h>

#include <GL/gl3w.h>>
#include <boost/filesystem.hpp>

#include <pixel_engine/program.h>

namespace pxl {
class OglTexture2d : public Texture2d {
 public:
  OglTexture2d(int32_t width, int32_t height, Format format,
               int32_t channels = 4);
  OglTexture2d(float* data, int32_t width, int32_t height,
               int32_t channels = 4);
  OglTexture2d(const boost::filesystem::path& path);
  ~OglTexture2d() override;

  void Bind() override;

  GLuint GetTextureId() const;
  void Use(uint32_t texture_unit) const;
  void SetSampler2D(const Program& prog, const std::string& name,
                    uint32_t texture_unit);
  void SetFilterMode(GLenum filter_mode) const;
  void SetWrapMode(GLenum wrap_mode) const;
  void SetBorder(float r, float g, float b, float a = 1);

 private:
  GLuint texture_id;
};
}  // namespace pxl
