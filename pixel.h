#ifndef HIPTEXT_PIXEL_H_
#define HIPTEXT_PIXEL_H_

#include <cstdint>
#include <glog/logging.h>

class Pixel {
 public:
  constexpr Pixel() : red_(0.0), green_(0.0), blue_(0.0), alpha_(1.0) {}
  constexpr Pixel(float red, float green, float blue, float alpha = 1.0)
      : red_(red), green_(green), blue_(blue), alpha_(alpha) {}
  inline float red() const { return red_; }
  inline float green() const { return green_; }
  inline float blue() const { return blue_; }
  inline float alpha() const { return alpha_; }
  inline float grey() const { return (red_ + green_ + blue_) / 3.0; }

 private:
  float red_;
  float green_;
  float blue_;
  float alpha_;
};

constexpr inline Pixel rgb256(int r, int g, int b) {
  return Pixel((float)r / 255.0, (float)g / 255.0, (float)b / 255.0);
}

constexpr inline Pixel grey256(int g) {
  return Pixel((float)g / 255.0, (float)g / 255.0, (float)g / 255.0);
}

#endif  // HIPTEXT_PIXEL_H_

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
