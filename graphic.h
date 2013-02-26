#ifndef HIPTEXT_GRAPHIC_H_
#define HIPTEXT_GRAPHIC_H_

#include <algorithm>
#include <utility>
#include <vector>
#include <glog/logging.h>

class Pixel;

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
        pixels_(std::move(pixels)) {}

  inline int width() const { return width_; }
  inline int height() const { return height_; }

  inline Pixel& Get(int x, int y) {
    DCHECK(0 <= x && x < width_) << "| x=" << x << " width_=" << width_;
    DCHECK(0 <= y && y < height_) << "| y=" << y << " height_=" << height_;
    return pixels_[y * width_ + x];
  }

  inline const Pixel& Get(int x, int y) const {
    DCHECK(0 <= x && x < width_) << "| x=" << x << " width_=" << width_;
    DCHECK(0 <= y && y < height_) << "| y=" << y << " height_=" << height_;
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

  Pixel GetAverageColor() const;
  Graphic Copy() const { return *this; }
  Graphic& Overlay(Graphic graphic, int offset_x = 0, int offset_y = 0);
  Graphic& Opacify(const Pixel& background);
  Graphic BilinearScale(int new_width, int new_height) const;

 private:
  int width_;
  int height_;
  std::vector<Pixel> pixels_;
};

#endif  // HIPTEXT_GRAPHIC_H_
