#pragma once

#include <cstdint>

#include <GL/gl3w.h>
#include <boost/filesystem.hpp>

namespace pxl {
class Texture2d {
 public:
  enum Format { INVALID, BYTE, FLOAT, DEPTH_STENCIL };

  Texture2d();
  Texture2d(const boost::filesystem::path& path);
  Texture2d(float* data, int32_t width, int32_t height, int32_t channels);
  Texture2d(int32_t width, int32_t height, Format format);
  virtual ~Texture2d() {}

  virtual void Bind(){};

  int32_t GetWidth() const;
  int32_t GetHeight() const;
  int32_t GetChannels() const;
  Format GetFormat() const;
  void Free();

 protected:
  float* image_data;
  int32_t width;
  int32_t height;
  int32_t channels;
  Format format;
};
}  // namespace pxl
