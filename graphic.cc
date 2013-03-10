// hiptext - Image to Text Converter
// By Justine Tunney

#include "graphic.h"
#include <cmath>
#include <iostream>
#include <glog/logging.h>
#include "pixel.h"

// Calculate number that's percent between p1 and p2.
static inline double Lerp(double p1, double p2, double percent) {
  return p1 * (1.0 - percent) + p2 * percent;
}

Graphic Graphic::BilinearScale(int new_width, int new_height) const {
  if (width_ == new_width && height_ == new_height) {
    return *this;
  }
  Graphic res(new_width, new_height);
  double rx = static_cast<double>(width_) / res.width_;
  double ry = static_cast<double>(height_) / res.height_;
  for (int y = 0; y < res.height_; ++y) {
    for (int x = 0; x < res.width_; ++x) {
      double sx = x * rx;
      double sy = y * ry;
      int fx = std::floor(sx);
      int fy = std::floor(sy);
      double px = sx - fx;
      double py = sy - fy;
      const Pixel& tl = SafeGet(fx, fy);
      const Pixel& tr = SafeGet(fx + 1, fy);
      const Pixel& bl = SafeGet(fx, fy + 1);
      const Pixel& br = SafeGet(fx + 1, fy + 1);
      res.Get(x, y) = Pixel(
          Lerp(Lerp(tl.red(), tr.red(), px),
               Lerp(bl.red(), br.red(), px), py),
          Lerp(Lerp(tl.green(), tr.green(), px),
               Lerp(bl.green(), br.green(), px), py),
          Lerp(Lerp(tl.blue(), tr.blue(), px),
               Lerp(bl.blue(), br.blue(), px), py),
          Lerp(Lerp(tl.alpha(), tr.alpha(), px),
               Lerp(bl.alpha(), br.alpha(), px), py));
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

Pixel Graphic::GetAverageColor(int x, int y, int w, int h) const {
  CHECK(0 <= x && x + w < width_);
  CHECK(0 <= y && y + h < height_);
  double avg_red = 0.0;
  double avg_green = 0.0;
  double avg_blue = 0.0;
  for (int dy = 0; dy < h; ++dy) {
    for (int dx = 0; dx < w; ++dx) {
      avg_red += Get(x + dx, y + dy).red();
      avg_green += Get(x + dx, y + dy).green();
      avg_blue += Get(x + dx, y + dy).blue();
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
