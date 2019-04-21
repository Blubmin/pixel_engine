#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/gl3w.h>>
#include <glog/logging.h>
#include <pixel_engine/camera.h>
#include <pixel_engine/game.h>
#include <pixel_engine/mesh_loader.h>
#include <pixel_engine/ogl_framebuffer.h>
#include <pixel_engine/ogl_fxaa_renderer.h>
#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/ogl_texture_renderer.h>
#include <pixel_engine/program.h>
#include <Eigen/Geometry>

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
    mesh = pxl::MeshLoader::LoadMesh<pxl::OglMesh>(GetMeshPath("bunny.obj"));
    mesh->Bind();

    prog = std::shared_ptr<pxl::Program>(new pxl::Program(
        GetShaderPath("mesh.vert"), GetShaderPath("mesh.frag")));
    camera.position += Eigen::Vector3f(0, -.2, 1);

    framebuffer = std::make_shared<pxl::OglFramebuffer>(1920, 1080);
    framebuffer->Bind();

    fxaa_output = std::make_shared<pxl::OglTexture2d>(
        1920, 1080, pxl::Texture2d::Format::FLOAT);
    fxaa_output->Bind();
  }
  void Loop() override {
    framebuffer->Start();
    prog->Bind();

    glUniformMatrix4fv(prog->GetUniformLocation("u_model"), 1, GL_FALSE,
                       mesh->GetTransform().data());
    glUniformMatrix4fv(prog->GetUniformLocation("u_view"), 1, GL_FALSE,
                       camera.GetView().data());
    glUniformMatrix4fv(prog->GetUniformLocation("u_perspective"), 1, GL_FALSE,
                       camera.GetPerspective().data());

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
};

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);
  HelloGame game;
  game.Run();
  return 0;
}
