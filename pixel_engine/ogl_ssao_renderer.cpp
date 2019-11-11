#include <pixel_engine/ogl_ssao_renderer.h>

#include <cmath>
#include <random>

#include <boost/format.hpp>

#include <pixel_engine/ogl_utilities.h>

namespace pxl {
namespace {
const uint32_t kKernelSize = 16;
const uint32_t kNoiseSize = 16;

float lerp(float a, float b, float t) { return a + t * (b - a); }

class SsaoBlurRenderer : public OglPostRenderer {
 public:
  SsaoBlurRenderer()
      : OglPostRenderer(boost::filesystem::path(__FILE__).parent_path() /
                        "shaders" / "ssao_blur.frag") {}

  static std::shared_ptr<SsaoBlurRenderer> GetInstance() {
    static std::shared_ptr<SsaoBlurRenderer> instance(nullptr);
    if (instance) {
      return instance;
    }
    instance = std::make_shared<SsaoBlurRenderer>();
    return instance;
  }

  void RenderTexture(const OglTexture2d& in_texture) {
    auto prog = GetProgram();

    prog->Bind();

    in_texture.Use(0);
    prog->SetUniform1i("u_ssao_texture", 0);

    DrawQuad();

    prog->UnBind();
  }
};
}  // namespace

OglSsaoRenderer::OglSsaoRenderer()
    : OglPostRenderer(boost::filesystem::path(__FILE__).parent_path() /
                      "shaders" / "ssao.frag") {}

std::shared_ptr<OglSsaoRenderer> OglSsaoRenderer::GetInstance() {
  static std::shared_ptr<OglSsaoRenderer> instance(nullptr);
  if (instance) {
    return instance;
  }
  instance = std::make_shared<OglSsaoRenderer>();

  // Generate Kernel
  std::uniform_real_distribution<float> random_float(
      0.0, 1.0);  // random floats between 0.0 - 1.0
  std::default_random_engine generator;
  for (uint32_t i = 0; i < kKernelSize; ++i) {
    Eigen::Vector3f sample(random_float(generator) * 2.f - 1.f,
                           random_float(generator) * 2.f - 1.f,
                           random_float(generator));
    sample.normalize();
    sample *= random_float(generator);
    float scale = float(i) / 64.f;
    scale = lerp(.1f, 1.f, scale * scale);
    sample *= scale;
    instance->kernel_.push_back(sample.x());
    instance->kernel_.push_back(sample.y());
    instance->kernel_.push_back(sample.z());
  }

  // Generate noise texture
  std::vector<float> noise_values;
  for (uint32_t i = 0; i < kNoiseSize; ++i) {
    noise_values.push_back(random_float(generator) * 2.f - 1.f);
    noise_values.push_back(random_float(generator) * 2.f - 1.f);
    noise_values.push_back(0.f);
  }
  instance->noise_ =
      std::make_shared<OglTexture2d>(noise_values.data(), 4, 4, 3);
  instance->noise_->Bind();
  instance->noise_->SetFilterMode(GL_NEAREST);
  instance->noise_->SetWrapMode(GL_REPEAT);

  // Create ssao framebuffer
  instance->ssao_framebuffer_ =
      std::make_shared<OglFramebuffer>(1920, 1080, std::vector<int32_t>({1}));
  instance->ssao_framebuffer_->Bind();
  instance->ssao_framebuffer_->SetClearColor(0, 0, 0, 1);

  return instance;
}

void OglSsaoRenderer::RenderTexture(const OglFramebuffer& g_buffer,
                                    const Camera& camera) {
  ssao_framebuffer_->SetClearColor(camera.far_plane, camera.far_plane,
                                   camera.far_plane);
  ssao_framebuffer_->Push();
  auto prog = GetProgram();

  prog->Bind();
  g_buffer.GetColorAttachment(1)->Use(0);
  prog->SetUniform1i("u_position_texture", 0);
  g_buffer.GetColorAttachment(2)->Use(1);
  prog->SetUniform1i("u_normal_texture", 1);

  prog->SetUniformMatrix4fv("u_view", camera.GetView().data());
  prog->SetUniformMatrix4fv("u_perspective", camera.GetPerspective().data());

  GLuint start = prog->GetUniformLocation("u_samples[0]");
  for (uint32_t i = 0; i < kernel_.size(); i += 3) {
    glUniform3fv(start + (i / 3), 1, &kernel_[i]);
  }
  noise_->Use(2);
  prog->SetUniform1i("u_ssao_noise", 2);

  int viewport_size[4];
  glGetIntegerv(GL_VIEWPORT, viewport_size);
  prog->SetUniform2f("u_noise_scale", viewport_size[2] / noise_->GetWidth(),
                     viewport_size[3] / noise_->GetHeight());
  DrawQuad();

  prog->UnBind();
  ssao_framebuffer_->Pop();

  SsaoBlurRenderer::GetInstance()->RenderTexture(
      *ssao_framebuffer_->GetColorAttachment(0));
}
}  // namespace pxl