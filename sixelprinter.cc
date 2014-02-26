// hiptext - Image to Text Converter
// By Justine Tunney

#include "sixelprinter.h"
#include "xterm256.h"
#include "pixel.h"

#include <cstring>
#include <iostream>

#include <glog/logging.h>

SixelPrinter::SixelPrinter(std::ostream& out, int colors,
                           bool is8bit, bool bgprint, int bg)
    :out_(out), colors_(colors), is8bit_(is8bit), bgprint_(bgprint)
    , bg_(bg), cache_(colors), count_(0), sixel_offset_(1) {
  if (colors > 2) {
    memset(slots_, 0, sizeof(slots_));
  }
};

void SixelPrinter::PrintPixel(int n) {
  char c;

  if (count_ == 0) {
    cache_ = n;
    count_ = 1;
  } else if (count_ < 0xff && cache_ == n) {
    ++count_;
  } else {
    if (colors_ > 2) {
      if (!bgprint_ && cache_ == bg_) {
        c = 0x3f;
      } else {
        c = 0x3f + sixel_offset_;
      }
      if (slots_[cache_] == 0) {
        DefineColor(cache_);
        slots_[cache_] = 1;  // set dirty
      }
      out_ << "#" << cache_; // choose color
    } else {
      if (cache_ == 0) {
        c = 0x3f;
      } else {
        c = 0x3f + sixel_offset_;
      }
    }
    cache_ = n;
    if (count_ > 2) {
      out_ << '!' << count_;
    } else if (count_ == 2) {
      out_ << c;
    }
    out_ << c;
    count_ = 1;
  }
}

void SixelPrinter::Start() {
  if (is8bit_) {
    out_ << '\x90';
  } else {
    out_ << "\033P";
  }
  out_ << "0;0;8q\"1;1";
}

void SixelPrinter::End() {
  if (sixel_offset_ != 1 << 5) {
    out_ << '-';
  }
  if (is8bit_) {
    out_ << '\x9c';
  } else {
    out_ << "\x1b\\";
  }
}

void SixelPrinter::LineFeed() {
  if (count_ > 1) {
    PrintPixel(colors_);
  }
  cache_ = 0;
  out_ << '$';
  if (sixel_offset_ == 1 << 5) {
    out_ << '-';
    sixel_offset_ = 1;
  } else {
    sixel_offset_ <<= 1;
  }
}

void SixelPrinter::DefineColor(int n) {
  const Pixel *pix = g_xterm + n;

  // emit a palette definition
  out_ << '#' << n  // specify palette No.
       << ';'
       << 2         // 1: use HLS palette 2: use RGB palette
       << ';'
       << static_cast<int>(pix->red() * 100)
       << ';'
       << static_cast<int>(pix->green() * 100)
       << ';'
       << static_cast<int>(pix->blue() * 100);
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
