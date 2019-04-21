#pragma once

#include <cstdint>

#include <GL/gl3w.h>

namespace pxl {
class Texture2d {
 public:
  enum Format { BYTE, FLOAT, DEPTH_STENCIL };

  Texture2d(uint32_t width, uint32_t height, Format format);
  virtual ~Texture2d() {}

  virtual void Bind(){};

  uint32_t GetWidth() const;
  uint32_t GetHeight() const;
  Format GetFormat() const;

 protected:
  uint32_t width;
  uint32_t height;
  Format format;
};
}  // namespace pxl