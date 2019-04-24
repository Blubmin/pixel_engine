#include <pixel_engine/scene.h>

namespace pxl {
namespace {
const int kGridSize = 10;
}
void Scene::Bind() {
  // Grid
  std::vector<Eigen::Vector2f> grid_positions;
  grid_positions.emplace_back(0, kGridSize);
  grid_positions.emplace_back(0, -kGridSize);
  grid_positions.emplace_back(kGridSize, 0);
  grid_positions.emplace_back(-kGridSize, 0);
  for (int i = 1; i <= kGridSize; ++i) {
    grid_positions.emplace_back(i, kGridSize);
    grid_positions.emplace_back(i, -kGridSize);
    grid_positions.emplace_back(-i, kGridSize);
    grid_positions.emplace_back(-i, -kGridSize);
    grid_positions.emplace_back(kGridSize, i);
    grid_positions.emplace_back(-kGridSize, i);
    grid_positions.emplace_back(kGridSize, -i);
    grid_positions.emplace_back(-kGridSize, -i);
  }
  glGenVertexArrays(1, &grid_vao_);
  glBindVertexArray(grid_vao_);
  glGenBuffers(1, &grid_buffer_);

  // Positions
  glBindBuffer(GL_ARRAY_BUFFER, grid_buffer_);
  glBufferData(GL_ARRAY_BUFFER,
               grid_positions.size() * sizeof(grid_positions[0]),
               grid_positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  // Pose
  std::vector<Eigen::Vector3f> pose_positions = {
      Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(1, 0, 0),
      Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 1, 0),
      Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 0, 1)};
  std::vector<Eigen::Vector3f> pose_colors = {
      Eigen::Vector3f(1, 0, 0), Eigen::Vector3f(1, 0, 0),
      Eigen::Vector3f(0, 1, 0), Eigen::Vector3f(0, 1, 0),
      Eigen::Vector3f(0, 0, 1), Eigen::Vector3f(0, 0, 1)};
  glGenVertexArrays(1, &pose_vao_);
  glBindVertexArray(pose_vao_);
  glGenBuffers(2, pose_buffers_);

  // Positions
  glBindBuffer(GL_ARRAY_BUFFER, pose_buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER,
               pose_positions.size() * sizeof(pose_positions[0]),
               pose_positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Colors
  glBindBuffer(GL_ARRAY_BUFFER, pose_buffers_[1]);
  glBufferData(GL_ARRAY_BUFFER, pose_colors.size() * sizeof(pose_colors[0]),
               pose_colors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Scene::DrawGrid() const {
  glBindVertexArray(grid_vao_);
  glLineWidth(2);
  glDrawArrays(GL_LINES, 0, (kGridSize * 2 + 1) * 4);
  glBindVertexArray(0);
}

void Scene::DrawPose() const {
  glBindVertexArray(pose_vao_);
  glLineWidth(2);
  glDrawArrays(GL_LINES, 0, 6);
  glBindVertexArray(0);
}
}  // namespace pxl
