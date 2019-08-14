#include <pixel_engine/ogl_texture_renderer.h>

#include <glog/logging.h>

namespace pxl {
OglTextureRenderer::OglTextureRenderer()
    : OglPostRenderer(boost::filesystem::path(__FILE__).parent_path() /
                      "shaders" / "texture.frag") {}

std::shared_ptr<OglTextureRenderer> OglTextureRenderer::GetInstance() {
  static std::shared_ptr<OglTextureRenderer> instance =
      std::make_shared<OglTextureRenderer>();
  return instance;
}

void OglTextureRenderer::RenderTexture(const OglTexture2d& texture,
                                       const Eigen::Rectf roi) {
  auto prog = GetProgram();
  if (prog == nullptr) {
    return;
  }

  prog->Bind();

  texture.Use(0);
  prog->SetUniform1i("texture_sampler", 0);

  GLint depth_func;
  glGetIntegerv(GL_DEPTH_FUNC, &depth_func);

  glDepthFunc(GL_ALWAYS);
  glDepthMask(0x00);
  DrawQuad(roi);
  glDepthMask(0xff);
  glDepthFunc(depth_func);

  prog->UnBind();
}
}  // namespace pxl
