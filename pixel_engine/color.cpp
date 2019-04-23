#include <pixel_engine/color.h>

namespace pxl {
Color::Color(float r, float g, float b, float a) : data_{r, g, b, a} {}

float Color::R() const { return data_[0]; }

float Color::G() const { return data_[1]; }

float Color::B() const { return data_[2]; }

float Color::A() const { return data_[3]; }

const float* Color::Data() const { return data_; }
}  // namespace pxl
