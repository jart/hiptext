#include "pixel.h"
#include <cmath>

const Pixel Pixel::kClear = Pixel(0.0, 0.0, 0.0, 0.0);
const Pixel Pixel::kBlack = Pixel(0.0, 0.0, 0.0, 1.0);
const Pixel Pixel::kWhite = Pixel(1.0, 1.0, 1.0, 1.0);

template<typename T>
constexpr inline T sqr(T val) {
  return val * val;
}

// Hue Saturation Lightness.
Pixel Pixel::HSL(float h, float s, float l, float a) {
  float m2 = (l < 0.5) ? l*(s+1) : 1+s-l*s;
  float m1 = l*2-m2;
  return Pixel(HueToRGB(m1, m2, h+1/3),
               HueToRGB(m1, m2, h    ),
               HueToRGB(m1, m2, h-1/3), a);
}

float Pixel::HueToRGB(float m1, float m2, float h) {
  if (h<0) h += 1;
  if (h>1) h -= 1;
  if (h*6<1) return m1+(m2-m1)*h*6;
  if (h*2<1) return m2;
  if (h*3<2) return m1+(m2-m1)*(2/3-h)*6;
  return m1;
}

float Pixel::Distance(const Pixel& other) const {
  return std::sqrt(sqr(red_ - other.red_) +
                   sqr(green_ - other.green_) +
                   sqr(blue_ - other.blue_));
}

Pixel Pixel::Opacify(const Pixel& background) const {
  if (alpha_ == 1.0) {
    return *this;
  } else {
    return Pixel(red_ * alpha_ + background.red_ * (1.0 - alpha_),
                 green_ * alpha_ + background.green_ * (1.0 - alpha_),
                 blue_ * alpha_ + background.blue_ * (1.0 - alpha_));
  }
}

std::string Pixel::ToString() const {
  std::string res(9, '#');
  snprintf(&res.front(), 10, "#%02x%02x%02x%02x",
           ColorTo256(red_),
           ColorTo256(green_),
           ColorTo256(blue_),
           ColorTo256(alpha_));
  return res;
}

std::ostream& operator<<(std::ostream& os, const Pixel& pixel) {
  os << pixel.ToString();
  return os;
}

// For Emacs:
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:2
// c-basic-offset:2
// c-file-style: nil
// End:
// For VIM:
// vim:set expandtab softtabstop=2 shiftwidth=2 tabstop=2:
