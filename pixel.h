#ifndef HIPTEXT_PIXEL_H_
#define HIPTEXT_PIXEL_H_

#include <cstdint>
#include <glog/logging.h>

struct Pixel {
  Pixel() {}
  Pixel(float r, float g, float b, float a = 1.0)
      : r(r), g(g), b(b), a(a) {}
  float r;
  float g;
  float b;
  float a;
  float grey() const { return (r + g + b) / 3.0 * a; }
};

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
