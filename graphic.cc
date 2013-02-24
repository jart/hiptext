#include "graphic.h"
#include <cmath>
#include <algorithm>
#include "pixel.h"

Graphic Graphic::BilinearScale(int new_width, int new_height) {
  Graphic res(new_width, new_height);
  double rx = (double)width_ / (double)res.width_;
  double ry = (double)height_ / (double)res.height_;
  for (int y = 0; y < res.height_; ++y) {
    for (int x = 0; x < res.width_; ++x) {
      double sx = x * rx;
      double sy = y * ry;
      int xl = std::min((int)std::floor(sx), width_ - 1);
      int xr = std::min((int)std::floor(sx + 1), width_ - 1);
      int yt = std::min((int)std::floor(sy), height_ - 1);
      int yb = std::min((int)std::floor(sy + 1), height_ - 1);
      const Pixel& tl = Get(xl, yt);
      const Pixel& tr = Get(xr, yt);
      const Pixel& bl = Get(xl, yb);
      const Pixel& br = Get(xr, yb);
      res.Get(x, y) = Pixel(
          (tl.red() + tr.red() + bl.red() + br.red()) / 4,
          (tl.green() + tr.green() + bl.green() + br.green()) / 4,
          (tl.blue() + tr.blue() + bl.blue() + br.blue()) / 4,
          (tl.alpha() + tr.alpha() + bl.alpha() + br.alpha()) / 4);
    }
  }
  return res;
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
