#include <pixel_engine/texture2d.h>

namespace pxl {
Texture2d::Texture2d(uint32_t width, uint32_t height, Texture2d::Format format)
    : width(width), height(height), format(format) {}

uint32_t Texture2d::GetWidth() const { return width; }

uint32_t Texture2d::GetHeight() const { return height; }

Texture2d::Format Texture2d::GetFormat() const { return format; }
}  // namespace pxl
