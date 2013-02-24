#ifndef HIPTEXT_PIXEL_H_
#define HIPTEXT_PIXEL_H_

#include <cstdint>
#include <glog/logging.h>

struct Pixel {
  constexpr Pixel() : r(0.0), g(0.0), b(0.0), a(1.0) {}
  constexpr Pixel(float r, float g, float b, float a = 1.0)
      : r(r), g(g), b(b), a(a) {}
  float grey() const { return (r + g + b) / 3.0 * a; }
  float r;
  float g;
  float b;
  float a;
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
