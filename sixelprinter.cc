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

  // TODO: more smart complession (now simple run-length compression only)
  if (count_ == 0) {
    cache_ = n;
    count_ = 1;
  } else if (count_ < 0xff && cache_ == n) {
    ++count_;
  } else {  // 16 color or 256 color
    if (colors_ > 2) {
      if (!bgprint_ && cache_ == bg_) {
        c = 0x3f;  // transparent pixel
      } else {
        c = 0x3f + sixel_offset_;
      }
      if (slots_[cache_] == 0) {
        DefineColor(cache_);
        slots_[cache_] = 1;  // set dirty
      }
      out_ << '#' << cache_; // choose color
    } else {  //  monochrome
      if (cache_ == 0) {
        c = 0x3f;  // transparent pixel
      } else {
        c = 0x3f + sixel_offset_;
      }
    }
    cache_ = n;
    if (count_ > 2) {
      out_ << '!'  // emit DECGRI (Graphics Repeat Introducer)
           << count_;
    } else if (count_ == 2) {
      out_ << c;
    }
    out_ << c;
    count_ = 1;
  }
}

void SixelPrinter::Start() {
  // emit a DCS (Device Control String) introducer
  if (is8bit_) {
    out_ << '\x90';
  } else {
    out_ << "\033P";
  }

  out_ << "0;0;8q" // emit SIXEL identifier
       << "\"1;1"; // specify aspect ratio
}

void SixelPrinter::End() {
  if (sixel_offset_ != 1) {
    out_ << '-';  // emit the last DECGNL (Graphics Next Line)
  }
  // emit a ST (String Terminator)
  if (is8bit_) {
    out_ << '\x9c';
  } else {
    out_ << "\x1b\\";
  }
}

void SixelPrinter::LineFeed() {
  if (count_ > 0) {
    PrintPixel(colors_);
  }

  if (sixel_offset_ == 1 << 5) {
    out_ << '$';  // for old mlterm (<= 3.3.1)
    out_ << '-';  // emit DECGNL (Graphics Next Line)
    sixel_offset_ = 1;  // reset sixel offset
  } else {
    out_ << '$';  // emit DECGCR (Graphics Carriage Return)
    sixel_offset_ <<= 1;  // increment sixel offset
  }

  // reset states
  count_ = 0;
  cache_ = colors_;
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
