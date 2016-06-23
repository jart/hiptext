// hiptext - Image to Text Converter
// By Justine Tunney

#ifndef HIPTEXT_GRAPHIC_H_
#define HIPTEXT_GRAPHIC_H_

#include <algorithm>
#include <utility>
#include <vector>
#include <glog/logging.h>

#include "hiptext/pixel.h"

class Graphic {
 public:
  Graphic(int width, int height)
    : width_(width),
      height_(height),
      pixels_(width * height) {}

  Graphic(int width, int height, const Pixel& pixel)
    : width_(width),
      height_(height),
      pixels_(width * height, pixel) {}

  Graphic(int width, int height, std::vector<Pixel>&& pixels)
      : width_(width),
        height_(height),
        pixels_(std::move(pixels)) {
    CHECK(width * height == (int)pixels_.size());
  }

  inline int width() const { return width_; }
  inline int height() const { return height_; }

  inline Pixel& Get(int x, int y) {
    DCHECK_GE(x, 0);
    DCHECK_LT(x, width_);
    DCHECK_GE(y, 0);
    DCHECK_LT(y, height_);
    return pixels_[y * width_ + x];
  }

  inline const Pixel& Get(int x, int y) const {
    DCHECK_GE(x, 0);
    DCHECK_LT(x, width_);
    DCHECK_GE(y, 0);
    DCHECK_LT(y, height_);
    return pixels_[y * width_ + x];
  }

  inline Pixel& SafeGet(int x, int y) {
    return pixels_[std::max(std::min(y, height_ - 1), 0) * width_ +
                   std::max(std::min(x, width_ - 1), 0)];
  }

  inline const Pixel& SafeGet(int x, int y) const {
    return pixels_[std::max(std::min(y, height_ - 1), 0) * width_ +
                   std::max(std::min(x, width_ - 1), 0)];
  }

  Pixel GetAverageColor(int x, int y, int w, int h) const;
  Graphic Copy() const { return *this; }
  Graphic& Overlay(Graphic graphic, int offset_x = 0, int offset_y = 0);
  Graphic& Opacify(const Pixel& background);
  Graphic BilinearScale(int new_width, int new_height) const;
  Graphic& Equalize();
  Graphic& ToYUV();
  Graphic& FromYUV();
  Graphic& ToHSV();
  Graphic& FromHSV();

 private:
  int width_;
  int height_;
  std::vector<Pixel> pixels_;
};

#endif  // HIPTEXT_GRAPHIC_H_
