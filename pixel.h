// hiptext - Image to Text Converter
// By Justine Tunney

#ifndef HIPTEXT_PIXEL_H_
#define HIPTEXT_PIXEL_H_

#include <string>
#include <ostream>

class Pixel {
 public:
  static const Pixel kClear;
  static const Pixel kBlack;
  static const Pixel kWhite;

  Pixel() = default;
  explicit Pixel(const std::string& name);

  constexpr Pixel(double red, double green, double blue, double alpha = 1.0)
      : red_(red),
        green_(green),
        blue_(blue),
        alpha_(alpha) {}

  constexpr Pixel(int red, int green, int blue, int alpha = 255)
      : red_(static_cast<double>(red) / 255.0),
        green_(static_cast<double>(green) / 255.0),
        blue_(static_cast<double>(blue) / 255.0),
        alpha_(static_cast<double>(alpha) / 255.0) {}

  inline double red() const { return red_; }
  inline double green() const { return green_; }
  inline double blue() const { return blue_; }
  inline double alpha() const { return alpha_; }
  inline double grey() const { return (red_ + green_ + blue_) / 3.0 * alpha_; }

  inline Pixel& set_red(double red) { red_ = red; return *this; }
  inline Pixel& set_green(double green) { green_ = green; return *this; }
  inline Pixel& set_blue(double blue) { blue_ = blue; return *this; }
  inline Pixel& set_alpha(double alpha) { alpha_ = alpha; return *this; }

  Pixel Copy() const { return *this; }
  Pixel& Overlay(const Pixel& other);
  Pixel& Opacify(const Pixel& background);
  Pixel& Mix(const Pixel& other);
  Pixel& MixKubelkaMunk(const Pixel& other);
  Pixel& ToHSV();
  Pixel& FromHSV();
  Pixel& FromHSL();

  double Distance(const Pixel& other) const;
  std::string ToString() const;

  bool operator==(const Pixel& other) const {
    return (red_ == other.red_ &&
            green_ == other.green_ &&
            blue_ == other.blue_ &&
            alpha_ == other.alpha_);
  }

  static inline Pixel HSV(double h, double s, double v, double a = 1.0) {
    return Pixel(h, s, v, a).FromHSV();
  }

  static inline Pixel HSV(int h, int s, int v, int a = 255) {
    return Pixel(h, s, v, a).FromHSV();
  }

  static inline Pixel HSL(double h, double s, double l, double a = 1.0) {
    return Pixel(h, s, l, a).FromHSL();
  }

  static inline Pixel HSL(int h, int s, int l, int a = 255) {
    return Pixel(h, s, l, a).FromHSL();
  }

 private:
  double red_;
  double green_;
  double blue_;
  double alpha_;
} __attribute__ ((aligned(16)));

std::ostream& operator<<(std::ostream& os, const Pixel& pixel);

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
