#include <pixel_engine/free_camera_component.h>

#include <GL/gl3w.h>
#include <glfw/glfw3.h>
#include <imgui/imgui.h>

#include <pixel_engine/entity.h>
#include <pixel_engine/game.h>

namespace pxl {
namespace {
float kMoveSpeed = 10.f;
float kRotationSpeed = 360.f;
}  // namespace
void FreeCameraComponent::Update(float time_elapsed) {
  auto entity = owner.lock();
  auto matrix = entity->GetTransform();

  Eigen::Vector3f right = matrix.block<3, 1>(0, 0);
  Eigen::Vector3f up = matrix.block<3, 1>(0, 1);
  Eigen::Vector3f forward = -matrix.block<3, 1>(0, 2);

  // Rotation
  if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT)) {
    entity->rotation.y() -=
        ImGui::GetIO().MouseDelta.x * kRotationSpeed / Game::State.window_width;
    entity->rotation.x() -= ImGui::GetIO().MouseDelta.y * kRotationSpeed /
                            Game::State.window_height;
    entity->rotation.x() = min(90.f, max(entity->rotation.x(), -90.f));
  }

  // Translation
  Eigen::Vector3f direction = Eigen::Vector3f::Zero();
  if (ImGui::IsKeyDown(GLFW_KEY_W)) {
    direction += forward;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_S)) {
    direction -= forward;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_D)) {
    direction += right;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_A)) {
    direction -= right;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_Q)) {
    direction += up;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_E)) {
    direction -= up;
  }
  direction.normalize();
  entity->position += direction * kMoveSpeed * time_elapsed;
}
}  // namespace pxl
