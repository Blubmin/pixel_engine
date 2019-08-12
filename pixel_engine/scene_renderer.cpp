#include <pixel_engine/scene_renderer.h>

#include <glog/logging.h>
#include <boost/format.hpp>

#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/point_light.h>
#include <pixel_engine/utilities.h>

namespace pxl {
std::shared_ptr<Program> SceneRenderer::mesh_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::grid_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::pose_prog_(nullptr);

void SceneRenderer::RenderScene(const Scene& scene) {
  if (grid_prog_ == nullptr) {
    Init();
  }

  if (scene.camera == nullptr) {
    LOG(ERROR) << "No camera set for scene.";
    return;
  }

  // Draw meshes
  RenderMeshes(scene);

  // Draw grid
  grid_prog_->Bind();
  grid_prog_->SetUniformMatrix4fv("u_perspective",
                                  scene.camera->GetPerspective().data());
  grid_prog_->SetUniformMatrix4fv("u_view", scene.camera->GetView().data());
  scene.DrawGrid();
  grid_prog_->UnBind();

  // Draw pose
  float viewport[4];
  glGetFloatv(GL_VIEWPORT, viewport);
  glViewport(0, 0, 100, 100);
  pose_prog_->Bind();
  auto view_matrix = scene.camera->GetView();
  view_matrix.block<3, 1>(0, 3) << 0.f, 0.f, 0.f;
  pose_prog_->SetUniformMatrix4fv("u_view", view_matrix.data());
  scene.DrawPose();
  pose_prog_->UnBind();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void SceneRenderer::RenderMeshes(const Scene& scene) {
  auto point_lights = scene.GetEntities<PointLight>();
  auto meshes = scene.GetEntities<OglMesh>();

  mesh_prog_->Bind();

  mesh_prog_->SetUniformMatrix4fv("u_view", scene.camera->GetView().data());
  mesh_prog_->SetUniformMatrix4fv("u_perspective",
                                  scene.camera->GetPerspective().data());

  mesh_prog_->SetUniform3fv("u_camera_pos", scene.camera->position.data());

  mesh_prog_->SetUniform1i("u_num_point_lights", point_lights.size());
  for (size_t i = 0; i < point_lights.size(); ++i) {
    auto point_light = point_lights.at(i);
    boost::format array_format("u_point_lights[%u].");
    array_format % i;
    mesh_prog_->SetUniform3fv(
        array_format.str() + "pos",
        point_light->GetTransform().block<3, 1>(0, 3).data());
    mesh_prog_->SetUniform3fv(array_format.str() + "color",
                              point_light->color.Data());
    mesh_prog_->SetUniform1f(array_format.str() + "linear_attenuation",
                             point_light->linear_attenuation);
    mesh_prog_->SetUniform1f(array_format.str() + "quadratic_attenuation",
                             point_light->quadratic_attenuation);
  }

  for (auto mesh : meshes) {
    mesh_prog_->SetUniformMatrix4fv("u_model", mesh->GetTransform().data());
    mesh->Draw();
  }
  mesh_prog_->UnBind();
}

void SceneRenderer::Init() {
  mesh_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "mesh.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "mesh.frag");
  grid_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "grid.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "grid.frag");
  pose_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "pose.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "pose.frag");
}
}  // namespace pxl
