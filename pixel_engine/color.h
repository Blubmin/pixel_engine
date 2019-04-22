#pragma once

namespace pxl {
class Color {
 public:
  Color(float r, float g, float b, float a = 1);

  float R() const;
  float G() const;
  float B() const;
  float A() const;
  const float* Data() const;

 private:
  float data_[4];
};
}  // namespace pxl
