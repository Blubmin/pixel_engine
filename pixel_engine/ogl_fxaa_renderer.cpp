#include <pixel_engine/ogl_fxaa_renderer.h>

namespace pxl {
OglFxaaRenderer::OglFxaaRenderer()
    : OglPostRenderer(boost::filesystem::path(__FILE__).parent_path() /
                      "shaders" / "fxaa.frag") {}

std::shared_ptr<OglFxaaRenderer> OglFxaaRenderer::GetInstance() {
  static std::shared_ptr<OglFxaaRenderer> instance =
      std::make_shared<OglFxaaRenderer>();
  return instance;
}

void OglFxaaRenderer::RenderTexture(const OglTexture2d& in_texture) {
  auto prog = GetProgram();
  if (prog == nullptr) {
    return;
  }

  prog->Bind();

  prog->SetUniform1i("width", in_texture.GetWidth());
  prog->SetUniform1i("height", in_texture.GetHeight());
  prog->SetUniform1i("in_image", 0);
  in_texture.Use(0);
  DrawQuad();

  prog->UnBind();
}
}  // namespace pxl
