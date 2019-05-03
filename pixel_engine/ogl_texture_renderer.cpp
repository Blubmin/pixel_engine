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

void OglTextureRenderer::RenderTexture(const OglTexture2d& texture) {
  auto prog = GetProgram();
  if (prog == nullptr) {
    return;
  }

  prog->Bind();

  texture.Use(0);
  prog->SetUniform1i("texture_sampler", 0);

  glDisable(GL_DEPTH);
  glDepthMask(0x00);
  DrawQuad();
  glDepthMask(0xff);
  glEnable(GL_DEPTH);

  prog->UnBind();
}
}  // namespace pxl
