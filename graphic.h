#ifndef HIPTEXT_GRAPHIC_H_
#define HIPTEXT_GRAPHIC_H_

#include <utility>
#include <vector>
#include <glog/logging.h>
#include "pixel.h"

class Graphic {
 public:
  Graphic(int width, int height)
    : width_(width),
      height_(height),
      pixels_(width * height) {}

  Graphic(int width, int height, std::vector<Pixel>&& pixels)
      : width_(width),
        height_(height),
        pixels_(std::move(pixels)) {}

  inline int width() const { return width_; }
  inline int height() const { return height_; }

  inline Pixel& Get(int x, int y) {
    DCHECK(0 <= x && x < width_);
    DCHECK(0 <= y && y < height_);
    return pixels_[y * width_ + x];
  }

  Graphic BilinearScale(int new_width, int new_height);

 private:
  int width_;
  int height_;
  std::vector<Pixel> pixels_;
};

#endif  // HIPTEXT_GRAPHIC_H_
