#include "pixel.h"
#include <cmath>
#include <glog/logging.h>

const Pixel Pixel::kClear = Pixel(0.0, 0.0, 0.0, 0.0);
const Pixel Pixel::kBlack = Pixel(0.0, 0.0, 0.0, 1.0);
const Pixel Pixel::kWhite = Pixel(1.0, 1.0, 1.0, 1.0);
const Pixel Pixel::kGreen = Pixel(0.0, 1.0, 0.0, 1.0);

Pixel& Pixel::FromHSV() {
  float chroma = green_ * blue_;
  float hdash = red_ * 6.0;
  float min = blue_ - chroma;
  float x = chroma * (1.0 - std::abs(std::fmod(hdash, 2.0f) - 1.0));
  if (hdash < 1.0) {
    red_ = chroma;
    green_ = x;
    blue_ = 0.0;
  } else if (hdash < 2.0) {
    red_ = x;
    green_ = chroma;
    blue_ = 0.0;
  } else if (hdash < 3.0) {
    red_ = 0.0;
    green_ = chroma;
    blue_ = x;
  } else if (hdash < 4.0) {
    red_ = 0.0;
    green_ = x;
    blue_ = chroma;
  } else if (hdash < 5.0) {
    red_ = x;
    green_ = 0.0;
    blue_ = chroma;
  } else if (hdash <= 6.0) {
    red_ = chroma;
    green_ = 0.0;
    blue_ = x;
  }
  red_ += min;
  green_ += min;
  blue_ += min;
  return *this;
}

Pixel& Pixel::ToHSV() {
  float min = std::min(std::min(red_, green_), blue_);
  float max = std::max(std::max(red_, green_), blue_);
  float chroma = max - min;
  if (chroma != 0.0) {
    if (red_ == max) {
      red_ = (green_ - blue_) / chroma;
      if (red_ < 0.0)
        red_ += 6.0;
    } else if (green_ == max) {
      red_ = ((blue_ - red_) / chroma) + 2.0;
    } else {
      red_ = ((red_ - green_) / chroma) + 4.0;
    }
    red_ /= 6.0;
    green_ = chroma / max;
  } else {
    red_ = 0.0;
    green_ = 0.0;
  }
  blue_ = max;
  return *this;
}

// Hue Saturation Luminosity
Pixel Pixel::HSL(float h, float s, float l, float a) {
  float m2 = (l <= 0.5) ? l*(s+1) : l+s-l*s;
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
  return std::sqrt(std::pow(red_ - other.red_, 2.0) +
                   std::pow(green_ - other.green_, 2.0) +
                   std::pow(blue_ - other.blue_, 2.0));
}

Pixel& Pixel::Overlay(const Pixel& other) {
  if (other.alpha_ == 0.0) {
    // Do nothing.
  } else if (other.alpha_ == 1.0) {
    *this = other;
  } else {
    // todo fix me
    red_ = red_ * (1.0 - other.alpha_) + other.red_ * other.alpha_;
    green_ = green_ * (1.0 - other.alpha_) + other.green_ * other.alpha_;
    blue_ = blue_ * (1.0 - other.alpha_) + other.blue_ * other.alpha_;
    alpha_ = 1.0;
  }
  return *this;
}

Pixel& Pixel::Opacify(const Pixel& background) {
  if (alpha_ == 1.0) {
    // Do nothing.
  } else if (alpha_ == 0.0) {
    *this = background;
  } else {
    red_ = red_ * alpha_ + background.red_ * (1.0 - alpha_);
    green_ = green_ * alpha_ + background.green_ * (1.0 - alpha_);
    blue_ = blue_ * alpha_ + background.blue_ * (1.0 - alpha_);
    alpha_ = 1.0;
  }
  return *this;
}

Pixel& Pixel::Mix(const Pixel& other) {
  red_ = (red_ + other.red_) / 2.0;
  green_ = (green_ + other.green_) / 2.0;
  blue_ = (blue_ + other.blue_) / 2.0;
  alpha_ = (alpha_ + other.alpha_) / 2.0;
  return *this;
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

// http://www.springerreference.com/docs/html/chapterdbid/212829.html
Pixel& Pixel::ToKubelkaMunk() {
  red_ = CalculateAbsorbance(red_);
  green_ = CalculateAbsorbance(green_);
  blue_ = CalculateAbsorbance(blue_);
  return *this;
}

Pixel& Pixel::FromKubelkaMunk() {
  red_ = CalculateReflectance(red_);
  green_ = CalculateReflectance(green_);
  blue_ = CalculateReflectance(blue_);
  return *this;
}

float Pixel::CalculateReflectance(float a) {
  // solve a = (1 - c)^2 / (2c), c
  return a - std::sqrt(a * (a + 2)) + 1.0;
  // return (1.0 + a - std::sqrt(std::pow(a, 2.0) + (2.0 * a)));
}

float Pixel::CalculateAbsorbance(float c) {
  // This is K/S part of the the equations on that website.
  // a = (1 - c)^2 / (2c)
  return std::pow(1.0 - c, 2.0) / (2.0 * c);
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
