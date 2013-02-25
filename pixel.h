#ifndef HIPTEXT_PIXEL_H_
#define HIPTEXT_PIXEL_H_

#include <cstdint>
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
  constexpr Pixel(float red, float green, float blue, float alpha = 1.0)
      : red_(red), green_(green), blue_(blue), alpha_(alpha) {}

  inline float red() const { return red_; }
  inline float green() const { return green_; }
  inline float blue() const { return blue_; }
  inline float alpha() const { return alpha_; }
  inline float grey() const { return (red_ + green_ + blue_) / 3.0 * alpha_; }

  inline void set_red(float red) { red_ = red; }
  inline void set_green(float green) { green_ = green; }
  inline void set_blue(float blue) { blue_ = blue; }
  inline void set_alpha(float alpha) { alpha_ = alpha; }

  Pixel Copy() const { return *this; }
  Pixel& Overlay(const Pixel& other);
  Pixel& Opacify(const Pixel& background);
  float Distance(const Pixel& other) const;
  std::string ToString() const;

  static Pixel Parse(const std::string& name);
  static Pixel HSL(float hue, float sat, float lit, float alpha = 1.0);
  static float HueToRGB(float m1, float m2, float h);

 private:
  float red_;
  float green_;
  float blue_;
  float alpha_;
};

std::ostream& operator<<(std::ostream& os, const Pixel& pixel);

template<typename T>
constexpr inline float Color256(T color) {
  static_assert(std::is_integral<T>::value, "color must be integer");
  return (float)color / 255.0;
}

constexpr inline uint8_t ColorTo256(float color) {
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
