#include <pixel_engine/sphere.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glog/logging.h>
#include <Eigen/Geometry>

namespace pxl {
UvSphere::UvSphere(float radius, uint32_t rows, uint32_t cols) {
  // Setup
  float u_step = 2 * M_PI / cols;
  float v_step = M_PI / rows;

  // Place side vertices
  for (uint32_t j = 1; j < rows; ++j) {
    Eigen::Vector3f pt(radius, 0, 0);
    pt =
        Eigen::AngleAxisf(M_PI_2 - (j * v_step), Eigen::Vector3f::UnitZ()) * pt;
    for (uint32_t i = 0; i < cols; ++i) {
      Eigen::Vector3f vert =
          Eigen::AngleAxisf(i * u_step, Eigen::Vector3f::UnitY()) * pt;
      positions.push_back(vert.x());
      positions.push_back(vert.y());
      positions.push_back(vert.z());
      normals.push_back(vert.normalized().x());
      normals.push_back(vert.normalized().y());
      normals.push_back(vert.normalized().z());
    }
  }

  // Top vertex
  positions.push_back(0);
  positions.push_back(radius);
  positions.push_back(0);
  normals.push_back(0);
  normals.push_back(1);
  normals.push_back(0);

  // Bottom Vertex
  positions.push_back(0);
  positions.push_back(-radius);
  positions.push_back(0);
  normals.push_back(0);
  normals.push_back(-1);
  normals.push_back(0);

  // Creates faces for sides
  for (int j = 0; j < rows - 2; ++j) {
    for (int i = 0; i < cols; ++i) {
      uint32_t start = j * cols;
      triangles.push_back(start + i);
      triangles.push_back(start + i + cols);
      triangles.push_back(start + cols + ((i + 1) % cols));
      triangles.push_back(start + i);
      triangles.push_back(start + cols + ((i + 1) % cols));
      triangles.push_back(start + ((i + 1) % cols));
    }
  }

  // Create top faces
  for (int i = 0; i < cols; ++i) {
    triangles.push_back((rows - 1) * cols);
    triangles.push_back(i);
    triangles.push_back((i + 1) % cols);
  }

  // Create bottom faces
  uint32_t final_row_start = (rows - 2) * cols;
  for (int i = 0; i < cols; ++i) {
    triangles.push_back(final_row_start + ((i + 1) % cols));
    triangles.push_back(final_row_start + i);
    triangles.push_back((rows - 1) * cols + 1);
  }
}
}  // namespace pxl