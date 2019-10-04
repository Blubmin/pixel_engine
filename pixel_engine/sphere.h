#pragma once

#include <pixel_engine/mesh.h>

namespace pxl {

struct UvSphere : SubMesh {
  UvSphere(float radius, uint32_t rows = 10, uint32_t cols = 20);
};
}  // namespace pxl