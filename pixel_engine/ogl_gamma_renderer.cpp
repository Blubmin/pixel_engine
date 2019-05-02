#include <pixel_engine/ogl_gamma_renderer.h>

namespace pxl {
OglGammaRenderer::OglGammaRenderer()
    : OglPostRenderer(boost::filesystem::path(__FILE__).parent_path() /
                      "shaders" / "gamma.frag") {}

std::shared_ptr<OglGammaRenderer> OglGammaRenderer::GetInstance() {
  static std::shared_ptr<OglGammaRenderer> instance =
      std::make_shared<OglGammaRenderer>();
  return instance;
}

void OglGammaRenderer::RenderTexture(std::shared_ptr<OglTexture2d> texture,
                                     float gamma) {
  auto prog = GetProgram();
  if (prog == nullptr) {
    return;
  }
  prog->Bind();
  prog->SetUniform1f("u_gamma", gamma);
  prog->SetUniform1i("u_texture", 0);
  texture->Use(0);
  DrawQuad();
  prog->UnBind();
}
}  // namespace pxl
