#include <pixel_engine/scene_renderer.h>

#include <glog/logging.h>
#include <boost/format.hpp>

#include <pixel_engine/directional_light.h>
#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/ogl_ssao_renderer.h>
#include <pixel_engine/ogl_utilities.h>
#include <pixel_engine/point_light.h>
#include <pixel_engine/utilities.h>

namespace pxl {
std::shared_ptr<OglFramebuffer> SceneRenderer::g_buffer_(nullptr);

std::shared_ptr<OglFramebuffer> SceneRenderer::ssao_buffer_(nullptr);

std::shared_ptr<Program> SceneRenderer::mesh_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::grid_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::pose_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::skybox_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::deferred_prog_(nullptr);

std::shared_ptr<OglPostRenderer> SceneRenderer::deferred_lighting_prog_(
    nullptr);

void SceneRenderer::RenderScene(const Scene& scene,
                                std::shared_ptr<OglFramebuffer> dst_framebuffer,
                                float gamma) {
  if (grid_prog_ == nullptr) {
    Init();
  }

  if (scene.camera == nullptr) {
    LOG(ERROR) << "No camera set for scene.";
    return;
  }

  // Draw meshes
  GLint output_framebuffer;
  g_buffer_->Begin();
  RenderGBuffers(scene, gamma);
  g_buffer_->End();

  // Draw ssao
  ssao_buffer_->Begin();
  OglSsaoRenderer::GetInstance()->RenderTexture(*g_buffer_, *scene.camera);
  ssao_buffer_->End();

  // Draw deferred lighting
  dst_framebuffer->Begin();
  RenderDeferredLighting(scene, gamma);
  dst_framebuffer->BlitDepth(*g_buffer_);

  // Draw Skybox
  skybox_prog_->Bind();
  if (scene.skybox != nullptr) {
    scene.skybox->position = scene.camera->position;
    scene.skybox->scale = Eigen::Vector3f::Ones() *
                          (scene.camera->far_plane - FLT_EPSILON) /
                          std::sqrt(3);
    skybox_prog_->SetUniformMatrix4fv("u_perspective",
                                      scene.camera->GetPerspective().data());
    skybox_prog_->SetUniformMatrix4fv("u_view", scene.camera->GetView().data());
    skybox_prog_->SetUniformMatrix4fv("u_model",
                                      scene.skybox->GetTransform().data());
    skybox_prog_->SetUniform1f("u_gamma", gamma);
    scene.skybox->mesh->Draw(*skybox_prog_);
  }
  skybox_prog_->UnBind();

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

void SceneRenderer::RenderGBuffers(const Scene& scene, float gamma) {
  auto meshes = scene.GetEntities<MeshEntity>();
  auto cameras = scene.GetEntities<Camera>();

  deferred_prog_->Bind();
  deferred_prog_->SetUniform1f("u_gamma", gamma);
  deferred_prog_->SetUniformMatrix4fv("u_perspective",
                                      scene.camera->GetPerspective().data());
  deferred_prog_->SetUniformMatrix4fv("u_view", scene.camera->GetView().data());

  // Draw Meshes
  for (auto mesh : meshes) {
    deferred_prog_->SetUniformMatrix4fv("u_model", mesh->GetTransform().data());
    mesh->Draw(*deferred_prog_);
  }

  // Draw Camera Meshes
  for (auto camera : cameras) {
    if (scene.camera == camera) {
      continue;
    }
    deferred_prog_->SetUniformMatrix4fv("u_model",
                                        camera->GetTransform().data());
    camera->Draw(*deferred_prog_);
  }

  deferred_prog_->UnBind();
}

void SceneRenderer::RenderDeferredLighting(const Scene& scene, float gamma) {
  auto prog = deferred_lighting_prog_->GetProgram();
  auto point_lights = scene.GetEntities<PointLight>();
  auto dir_lights = scene.GetEntities<DirectionalLight>();

  prog->Bind();

  prog->SetUniform1f("u_gamma", gamma);
  prog->SetUniform3fv("u_camera_pos", scene.camera->position.data());
  prog->SetUniform1i("u_num_point_lights", point_lights.size());

  // Point lights
  for (size_t i = 0; i < point_lights.size(); ++i) {
    auto point_light = point_lights.at(i);
    boost::format array_format("u_point_lights[%u].");
    array_format % i;
    prog->SetUniform3fv(array_format.str() + "pos",
                        point_light->GetTransform().block<3, 1>(0, 3).data());
    prog->SetUniform3fv(array_format.str() + "color",
                        point_light->color.Data());
    prog->SetUniform1f(array_format.str() + "linear_attenuation",
                       point_light->linear_attenuation);
    prog->SetUniform1f(array_format.str() + "quadratic_attenuation",
                       point_light->quadratic_attenuation);
  }

  // Only supports one directional light at the moment;
  if (!dir_lights.empty()) {
    auto dir_light = dir_lights[0];
    prog->SetUniform3fv("u_dir_light.direction", dir_light->direction.data());
    prog->SetUniform1f("u_dir_light.strength", dir_light->strength);
    prog->SetUniform3fv("u_dir_light.color", dir_light->color.data());
  } else {
    prog->SetUniform1f("u_dir_light.strength", 0);
  }

  g_buffer_->GetColorAttachment(0)->Use(0);
  prog->SetUniform1i("u_albedo_spec_texture", 0);
  g_buffer_->GetColorAttachment(1)->Use(1);
  prog->SetUniform1i("u_position_texture", 1);
  g_buffer_->GetColorAttachment(2)->Use(2);
  prog->SetUniform1i("u_normal_texture", 2);
  ssao_buffer_->GetColorAttachment(0)->Use(3);
  prog->SetUniform1i("u_ssao_texture", 3);

  deferred_lighting_prog_->DrawQuad();
  prog->UnBind();
}

void SceneRenderer::Init() {
  glDisable(GL_BLEND);
  g_buffer_ = std::make_shared<OglFramebuffer>(1920, 1080, 4);
  g_buffer_->Bind();
  ssao_buffer_ = std::make_shared<OglFramebuffer>(1920, 1080);
  ssao_buffer_->Bind();
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
  skybox_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "skybox.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "skybox.frag");
  deferred_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "mesh.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "mesh_deferred.frag");
  deferred_lighting_prog_ = std::make_shared<OglPostRenderer>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
      "deferred_lighting.frag");
}
}  // namespace pxl
