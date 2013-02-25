#include "graphic.h"
#include <cmath>
#include <algorithm>
#include "pixel.h"

Graphic Graphic::BilinearScale(int new_width, int new_height) const {
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

Graphic& Graphic::Overlay(Graphic graphic, int offset_x, int offset_y) {
  for (int y1 = offset_y, y2 = 0;
       y1 < height_ && y2 < graphic.height_;
       ++y1, ++y2) {
    for (int x1 = offset_x, x2 = 0;
         x1 < width_ && x2 < graphic.width_;
         ++x1, ++x2) {
      Get(x1, y1).Overlay(graphic.Get(x2, y2));
    }
  }
  return *this;
}

Graphic& Graphic::Opacify(const Pixel& background) {
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      Get(x, y).Opacify(background);
    }
  }
  return *this;
}

Pixel Graphic::GetAverageColor() const {
  double avg_red = 0.0;
  double avg_green = 0.0;
  double avg_blue = 0.0;
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      avg_red += Get(x, y).red();
      avg_green += Get(x, y).green();
      avg_blue += Get(x, y).blue();
    }
  }
  return Pixel(avg_red / (width_ * height_),
               avg_green / (width_ * height_),
               avg_blue / (width_ * height_));
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
