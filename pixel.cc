// hiptext - Image to Text Converter
// By Justine Tunney

#include "pixel.h"
#include <algorithm>
#include <cmath>
#include <glog/logging.h>

const Pixel Pixel::kClear = {   0,   0,   0,   0 };
const Pixel Pixel::kBlack = {   0,   0,   0, 255 };
const Pixel Pixel::kWhite = { 255, 255, 255, 255 };

Pixel& Pixel::FromHSV() {
  double chroma = green_ * blue_;
  double hdash = red_ * 6.0;
  double min = blue_ - chroma;
  double x = chroma * (1.0 - std::abs(std::fmod(hdash, 2.0) - 1.0));
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
  double min = std::min(std::min(red_, green_), blue_);
  double max = std::max(std::max(red_, green_), blue_);
  double chroma = max - min;
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

static double HueToRGB(double m1, double m2, double h) {
  if (h < 0) h += 1;
  if (h > 1) h -= 1;
  if (h*6 < 1) return m1+(m2-m1)*h*6;
  if (h*2 < 1) return m2;
  if (h*3 < 2) return m1+(m2-m1)*(2/3-h)*6;
  return m1;
}

// Hue Saturation Luminosity
Pixel& Pixel::FromHSL() {
  double m2 = ((blue_ <= 0.5)
               ? blue_ * (green_ + 1)
               : blue_ + green_ - blue_ * green_);
  double m1 = blue_ * 2 - m2;
  blue_ = HueToRGB(m1, m2, red_ - 1/3);
  green_ = HueToRGB(m1, m2, red_);
  red_ = HueToRGB(m1, m2, red_ + 1/3);
  return *this;
}

double Pixel::Distance(const Pixel& other) const {
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

// Layer this color on top of an opaque background, thus making it opaque.
Pixel& Pixel::Opacify(const Pixel& background) {
  DCHECK_EQ(1.0, background.alpha_);
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

// Naive RGB color mixing algorithm.
Pixel& Pixel::Mix(const Pixel& other) {
  red_ = (red_ + other.red_) / 2.0;
  green_ = (green_ + other.green_) / 2.0;
  blue_ = (blue_ + other.blue_) / 2.0;
  alpha_ = (alpha_ + other.alpha_) / 2.0;
  return *this;
}

// http://www.springerreference.com/docs/html/chapterdbid/212829.html
static double A(double c) {
  // This is K/S part of the the equations on that website.
  // a = (1 - c)^2 / (2c)
  return std::pow(1.0 - c, 2.0) / (2.0 * std::max(c, 1e-6));
}

// Inverse of A() : solve a = (1 - c)^2 / (2c), c
static double R(double a) {
  return a - std::sqrt(a * (a + 2)) + 1.0;
}

// Expensive opaque color mixing that's better for human eyes.
Pixel& Pixel::MixKubelkaMunk(const Pixel& other) {
  DCHECK_EQ(1.0, alpha_);
  DCHECK_EQ(1.0, other.alpha_);
  red_ = R(A(red_) + A(other.red_) / 2);
  green_ = R(A(green_) + A(other.green_) / 2);
  blue_ = R(A(blue_) + A(other.blue_) / 2);
  return *this;
}

std::string Pixel::ToString() const {
  std::string res(9, '#');
  snprintf(&res.front(), res.size() + 1, "#%02x%02x%02x%02x",
           static_cast<int>(red_ * 255),
           static_cast<int>(green_ * 255),
           static_cast<int>(blue_ * 255),
           static_cast<int>(alpha_ * 255));
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
