#ifndef HIPTEXT_PIXEL_H_
#define HIPTEXT_PIXEL_H_

#include <cstdint>
#include <functional>
#include <string>
#include <ostream>
#include <type_traits>

class Pixel {
 public:
  static const Pixel kClear;
  static const Pixel kBlack;
  static const Pixel kWhite;
  static const Pixel kGreen;

  constexpr Pixel() : red_(0.0), green_(0.0), blue_(0.0), alpha_(0.0) {}
  constexpr Pixel(double red, double green, double blue, double alpha = 1.0)
      : red_(red), green_(green), blue_(blue), alpha_(alpha) {}

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

  double Distance(const Pixel& other) const;
  std::string ToString() const;

  static Pixel Parse(const std::string& name);
  static Pixel HSL(double hue, double sat, double lum, double alpha = 1.0);

  bool operator==(const Pixel& other) const {
    return (red_ == other.red_ &&
            green_ == other.green_ &&
            blue_ == other.blue_ &&
            alpha_ == other.alpha_);
  }

 private:
  static double HueToRGB(double m1, double m2, double h);

  double red_;
  double green_;
  double blue_;
  double alpha_;
};

std::ostream& operator<<(std::ostream& os, const Pixel& pixel);

template<typename T>
constexpr inline double Color256(T color) {
  static_assert(std::is_integral<T>::value, "color must be integer");
  return (double)color / 255.0;
}

constexpr inline uint8_t ColorTo256(double color) {
  return (uint8_t)(color * 255.0);
}

constexpr inline Pixel RGB256(int r, int g, int b) {
  return Pixel(Color256(r), Color256(g), Color256(b), 1.0);
}

constexpr inline Pixel Grey256(int g) {
  return Pixel(Color256(g), Color256(g), Color256(g), 1.0);
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
