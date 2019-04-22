#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/gl3w.h>>
#include <glog/logging.h>
#include <imgui/imgui.h>
#include <pixel_engine/camera.h>
#include <pixel_engine/game.h>
#include <pixel_engine/mesh_loader.h>
#include <pixel_engine/ogl_framebuffer.h>
#include <pixel_engine/ogl_fxaa_renderer.h>
#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/ogl_texture_renderer.h>
#include <pixel_engine/point_light.h>
#include <pixel_engine/program.h>
#include <Eigen/Geometry>
#include <boost/format.hpp>

namespace {
boost::filesystem::path GetResourcePath() {
  return boost::filesystem::path(__FILE__).parent_path() / "resources";
}
boost::filesystem::path GetMeshPath(const std::string& mesh_file) {
  return GetResourcePath() / "meshes" / mesh_file;
}
boost::filesystem::path GetShaderPath(const std::string& shader_file) {
  return GetResourcePath() / "shaders" / shader_file;
}
}  // namespace

class HelloGame : public pxl::Game {
 public:
  HelloGame() : pxl::Game("Hello Game") {}
  void Init() override {
    mesh =
        pxl::MeshLoader::LoadMesh<pxl::OglMesh>(GetMeshPath("coca_cola.obj"));
    mesh->Bind();
    mesh->scale = Eigen::Vector3f(.01, .01, .01);

    prog = std::shared_ptr<pxl::Program>(new pxl::Program(
        GetShaderPath("mesh.vert"), GetShaderPath("mesh.frag")));
    camera.position += Eigen::Vector3f(0, .2, 5);

    framebuffer = std::make_shared<pxl::OglFramebuffer>(1920, 1080);
    framebuffer->Bind();

    fxaa_output = std::make_shared<pxl::OglTexture2d>(
        1920, 1080, pxl::Texture2d::Format::FLOAT);
    fxaa_output->Bind();

    point_lights.emplace_back(pxl::Color(1, .8, .8));
    point_lights.back().position += Eigen::Vector3f(2, 2, 0);
    point_lights.emplace_back(pxl::Color(.8, .8, 1));
    point_lights.back().position += Eigen::Vector3f(-2, 2, 0);
  }
  void Loop() override {
    framebuffer->Start();
    prog->Bind();

    mesh->rotation += Eigen::Vector3f(0, M_PI / 120, 0);
    if (ImGui::Begin("Property")) {
      ImGui::DragFloat3("Position", mesh->position.data());
      ImGui::DragFloat3("Rotation", mesh->rotation.data());
      ImGui::DragFloat3("Scale", mesh->scale.data());
      ImGui::End();
    }
    glUniformMatrix4fv(prog->GetUniformLocation("u_model"), 1, GL_FALSE,
                       mesh->GetTransform().data());
    glUniformMatrix4fv(prog->GetUniformLocation("u_view"), 1, GL_FALSE,
                       camera.GetView().data());
    glUniformMatrix4fv(prog->GetUniformLocation("u_perspective"), 1, GL_FALSE,
                       camera.GetPerspective().data());

    glUniform3fv(prog->GetUniformLocation("u_camera_pos"), 1,
                 camera.position.data());

    glUniform1i(prog->GetUniformLocation("u_num_point_lights"),
                point_lights.size());
    for (size_t i = 0; i < point_lights.size(); ++i) {
      const auto& point_light = point_lights.at(i);
      boost::format array_format("u_point_lights[%u].");
      array_format % i;
      glUniform3fv(prog->GetUniformLocation(array_format.str() + "pos"), 1,
                   point_light.position.data());
      glUniform3fv(prog->GetUniformLocation(array_format.str() + "color"), 1,
                   point_light.color.Data());
      glUniform1fv(
          prog->GetUniformLocation(array_format.str() + "linear_attenuation"),
          1, &point_light.linear_attenuation);
      glUniform1fv(prog->GetUniformLocation(array_format.str() +
                                            "quadratic_attenuation"),
                   1, &point_light.quadratic_attenuation);
    }

    mesh->Draw();
    prog->UnBind();
    framebuffer->End();

    pxl::OglFxaaRenderer::RenderTexture(*framebuffer->GetColorAttachment(0));
  }

  std::shared_ptr<pxl::OglFramebuffer> framebuffer;
  std::shared_ptr<pxl::OglTexture2d> fxaa_output;
  pxl::Camera camera;
  std::shared_ptr<pxl::OglMesh> mesh;
  std::shared_ptr<pxl::Program> prog;
  std::vector<pxl::PointLight> point_lights;
};

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);
  HelloGame game;
  game.Run();
  return 0;
}
