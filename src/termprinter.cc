// hiptext - Image to Text Converter
// By Justine Tunney

#include "hiptext/termprinter.h"

#include <cstring>
#include <iostream>

#include <glog/logging.h>

const int TermPrinter::kReset = 0;
const int TermPrinter::kBoldOn = 1;
const int TermPrinter::kBoldOff = 22;
const int TermPrinter::kItalicOn = 3;
const int TermPrinter::kItalicOff = 23;
const int TermPrinter::kUnderlineOn = 4;
const int TermPrinter::kUnderlineOff = 24;
const int TermPrinter::kUnderline2On = 21;
const int TermPrinter::kUnderline2Off = 24;
const int TermPrinter::kStrikeOn = 9;
const int TermPrinter::kStrikeOff = 29;
const int TermPrinter::kBlinkOn = 5;
const int TermPrinter::kBlinkOff = 25;
const int TermPrinter::kFlipOn = 7;
const int TermPrinter::kFlipOff = 27;
const int TermPrinter::kForegroundOff = 39;
const int TermPrinter::kBackgroundOff = 49;
const int TermPrinter::kForeground256 = (38 << 16) | (5 << 8);
const int TermPrinter::kBackground256 = (48 << 16) | (5 << 8);
const char* TermPrinter::kEscapeStart = "\x1b[";
const char* TermPrinter::kEscapeEnd = "m";
const char* TermPrinter::kEscapeSep = ";";
const char* TermPrinter::kEscapeReset = "\x1b[0m";

TermPrinter::TermPrinter(std::ostream& out) : out_(out) {
  memset(&cur_, 0, sizeof(cur_));
  memset(&new_, 0, sizeof(cur_));
}

bool TermPrinter::IsStyled() const {
  return (new_.fg ||
          new_.bg ||
          new_.bold ||
          new_.italic ||
          new_.underline ||
          new_.underline2 ||
          new_.strike ||
          new_.blink ||
          new_.flip);
}

void TermPrinter::Reset(bool force) {
  if (force || IsStyled()) {
    memset(&cur_, 0, sizeof(cur_));
    memset(&new_, 0, sizeof(cur_));
    out_ << kEscapeReset;
  }
}

void TermPrinter::Flush() {
  if (!dirty_)
    return;
  dirty_ = false;
  if (!new_.fg &&
      !new_.bg &&
      !new_.bold &&
      !new_.italic &&
      !new_.underline &&
      !new_.underline2 &&
      !new_.strike &&
      !new_.blink &&
      !new_.flip) {
    out_ << kEscapeReset;
    return;
  }
  out_ << kEscapeStart;

  bool first = true;

  if (new_.bold != cur_.bold) {
    PrintSep(&first);
    out_ << (new_.bold ? kBoldOn : kBoldOff);
    cur_.bold = new_.bold;
  }

  if (new_.italic != cur_.italic) {
    PrintSep(&first);
    out_ << (new_.italic ? kItalicOn : kItalicOff);
    cur_.italic = new_.italic;
  }

  if (new_.underline != cur_.underline) {
    PrintSep(&first);
    out_ << (new_.underline ? kUnderlineOn : kUnderlineOff);
    cur_.underline = new_.underline;
  }

  if (new_.underline2 != cur_.underline2) {
    PrintSep(&first);
    out_ << (new_.underline2 ? kUnderline2On : kUnderline2Off);
    cur_.underline2 = new_.underline2;
  }

  if (new_.strike != cur_.strike) {
    PrintSep(&first);
    out_ << (new_.strike ? kStrikeOn : kStrikeOff);
    cur_.strike = new_.strike;
  }

  if (new_.blink != cur_.blink) {
    PrintSep(&first);
    out_ << (new_.blink ? kBlinkOn : kBlinkOff);
    cur_.blink = new_.blink;
  }

  if (new_.flip != cur_.flip) {
    PrintSep(&first);
    out_ << (new_.flip ? kFlipOn : kFlipOff);
    cur_.flip = new_.flip;
  }

  if (new_.fg != cur_.fg) {
    if (new_.fg == 0) {
      PrintSep(&first);
      out_ << kForegroundOff;
    } else {
      PrintCode(new_.fg, &first);
    }
    cur_.fg = new_.fg;
  }

  if (new_.bg != cur_.bg) {
    if (new_.bg == 0) {
      PrintSep(&first);
      out_ << kBackgroundOff;
    } else {
      PrintCode(new_.bg, &first);
    }
    cur_.bg = new_.bg;
  }

  out_ << kEscapeEnd;
}

void TermPrinter::SetBold(bool bold) {
  if (bold != new_.bold) {
    new_.bold = bold;
    dirty_ = true;
  }
}

void TermPrinter::SetItalic(bool italic) {
  if (italic != new_.italic) {
    new_.italic = italic;
    dirty_ = true;
  }
}

void TermPrinter::SetUnderline(bool underline) {
  if (underline != new_.underline) {
    new_.underline = underline;
    dirty_ = true;
  }
}

void TermPrinter::SetUnderline2(bool underline2) {
  if (underline2 != new_.underline2) {
    new_.underline2 = underline2;
    dirty_ = true;
  }
}

void TermPrinter::SetStrike(bool strike) {
  if (strike != new_.strike) {
    new_.strike = strike;
    dirty_ = true;
  }
}

void TermPrinter::SetBlink(bool blink) {
  if (blink != new_.blink) {
    new_.blink = blink;
    dirty_ = true;
  }
}

void TermPrinter::SetFlip(bool flip) {
  if (flip != new_.flip) {
    new_.flip = flip;
    dirty_ = true;
  }
}

void TermPrinter::SetForeground256(int code) {
  if (code != 0) {
    code |= kForeground256;
  }
  if (code != new_.fg) {
    new_.fg = code;
    dirty_ = true;
  }
}

void TermPrinter::SetBackground256(int code) {
  if (code != 0) {
    code |= kBackground256;
  }
  if (code != new_.bg) {
    new_.bg = code;
    dirty_ = true;
  }
}

void TermPrinter::PrintSep(bool* first) const {
  CHECK_NOTNULL(first);
  if (*first) {
    *first = false;
  } else {
    out_ << kEscapeSep;
  }
}

void TermPrinter::PrintCode(int code, bool* first) {
  CHECK_NOTNULL(first);
  int part;
  part = (code >> 16) & 0xff;
  if (part) {
    PrintSep(first);
    out_ << part;
  }
  part = (code >> 8) & 0xff;
  if (part) {
    PrintSep(first);
    out_ << part;
  }
  part = code & 0xff;
  if (part) {
    PrintSep(first);
    out_ << part;
  }
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
