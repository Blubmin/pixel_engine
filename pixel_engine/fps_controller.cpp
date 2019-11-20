#include <pixel_engine/fps_controller.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glfw/glfw3.h>
#include <imgui/imgui.h>

#include <pixel_engine/camera.h>
#include <pixel_engine/entity.h>
#include <pixel_engine/game.h>
#include <pixel_engine/physics_component.h>

#define CLAMP(A, T, B) ((T) < (A) ? (A) : (T) > (B) ? (B) : (T))

namespace pxl {
FpsController::FpsController() : speed(4), disable(false) {}

void FpsController::Update(float time_elapsed) {
  if (disable) {
    return;
  }
  auto owner_ptr = owner.lock();
  if (owner_ptr == nullptr) {
    return;
  }
  Rotate();
  owner_ptr->position += GetMovementVector() * speed * time_elapsed;

  auto physics = owner_ptr->GetComponent<PhysicsComponent>();
  physics->velocity.x() = 0;
  physics->velocity.z() = 0;
  physics->velocity += GetMovementVector() * speed;
  if (ImGui::IsKeyPressed(GLFW_KEY_SPACE) && physics != nullptr) {
    if (physics->velocity.y() == 0) {
      physics->velocity.y() = 6.f;
    }
  }
}

void FpsController::Rotate() {
  auto owner_ptr = owner.lock();

  ImVec2 delta = ImGui::GetIO().MouseDelta;

  owner_ptr->rotation.y() -= delta.x / Game::State.window_height * 360;

  // Rotate cameras in x
  auto child = owner_ptr->GetChild<Camera>();
  if (child != nullptr) {
    child->rotation.x() -= delta.y / Game::State.window_width * 180;
    child->rotation.x() = CLAMP(-90, child->rotation.x(), 90);
  }
}

Eigen::Vector3f FpsController::GetMovementVector() const {
  auto owner_ptr = owner.lock();
  Eigen::Vector3f forward = owner_ptr->GetTransform().block<3, 1>(0, 2);
  forward = Eigen::Vector3f(forward.x(), 0, forward.z()).normalized();
  Eigen::Vector3f right = -owner_ptr->GetTransform().block<3, 1>(0, 0);

  Eigen::Vector3f velocity = Eigen::Vector3f::Zero();

  if (ImGui::IsKeyDown(GLFW_KEY_W)) {
    velocity += forward;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_S)) {
    velocity -= forward;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_A)) {
    velocity -= right;
  }
  if (ImGui::IsKeyDown(GLFW_KEY_D)) {
    velocity += right;
  }
  return velocity.normalized();
}

}  // namespace pxl