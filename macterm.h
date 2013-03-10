// hiptext - Image to Text Converter
// Copyright (c) 2013 Justine Tunney

#ifndef HIPTEXT_MACTERM_H_
#define HIPTEXT_MACTERM_H_

#include <cstdint>
#include <utility>

class Pixel;

class MactermColor {
 public:
  MactermColor(const Pixel& top, const Pixel& bot);
  inline uint8_t bg() const { return bg_; }
  inline uint8_t fg() const { return fg_; }
  inline wchar_t symbol() const { return symbol_; }

 private:
  static const wchar_t kUpperHalfBlock = L'\u2580';
  static const wchar_t kLowerHalfBlock = L'\u2584';
  static const wchar_t kFullBlock = L'\u2588';
  static const wchar_t kSpace = L' ';

  uint8_t bg_;
  uint8_t fg_;
  wchar_t symbol_;
};

extern const Pixel macterm_colors[2][256];

#endif  // HIPTEXT_MACTERM_H_

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
