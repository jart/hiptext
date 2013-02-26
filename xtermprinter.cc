#include "xtermprinter.h"

#include <cstring>
#include <iostream>

#include "xterm256.h"

const int XtermPrinter::kReset = 0;
const int XtermPrinter::kBoldOn = 1;
const int XtermPrinter::kBoldOff = 22;
const int XtermPrinter::kItalicOn = 3;
const int XtermPrinter::kItalicOff = 23;
const int XtermPrinter::kUnderlineOn = 4;
const int XtermPrinter::kUnderlineOff = 24;
const int XtermPrinter::kUnderline2On = 21;
const int XtermPrinter::kUnderline2Off = 24;
const int XtermPrinter::kStrikeOn = 9;
const int XtermPrinter::kStrikeOff = 29;
const int XtermPrinter::kBlinkOn = 5;
const int XtermPrinter::kBlinkOff = 25;
const int XtermPrinter::kFlipOn = 7;
const int XtermPrinter::kFlipOff = 27;
const int XtermPrinter::kForegroundOff = 39;
const int XtermPrinter::kBackgroundOff = 49;
const int XtermPrinter::kForeground256 = (38 << 16) | (5 << 8);
const int XtermPrinter::kBackground256 = (48 << 16) | (5 << 8);
const std::string XtermPrinter::kEscapeStart = "\x1b[";
const std::string XtermPrinter::kEscapeEnd = "m";
const std::string XtermPrinter::kEscapeSep = ";";
const std::string XtermPrinter::kEscapeReset = "\x1b[0m";

XtermPrinter::XtermPrinter(std::ostream* out, const Pixel& bg, bool bgprint)
    : bg_(bg),
      bg256_(rgb_to_xterm256(bg)),
      bgprint_(bgprint),
      out_(out) {
  memset(&cur_, 0, sizeof(cur_));
  memset(&new_, 0, sizeof(cur_));
}

bool XtermPrinter::IsStyled() const {
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

void XtermPrinter::Reset(bool force) {
  if (force || IsStyled()) {
    memset(&cur_, 0, sizeof(cur_));
    memset(&new_, 0, sizeof(cur_));
    (*out_) << kEscapeReset;
  }
}

void XtermPrinter::Flush() {
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
    (*out_) << kEscapeReset;
    return;
  }
  (*out_) << kEscapeStart;

  bool first = true;

  if (new_.bold != cur_.bold) {
    PrintSep(&first);
    (*out_) << (new_.bold ? kBoldOn : kBoldOff);
    cur_.bold = new_.bold;
  }

  if (new_.italic != cur_.italic) {
    PrintSep(&first);
    (*out_) << (new_.italic ? kItalicOn : kItalicOff);
    cur_.italic = new_.italic;
  }

  if (new_.underline != cur_.underline) {
    PrintSep(&first);
    (*out_) << (new_.underline ? kUnderlineOn : kUnderlineOff);
    cur_.underline = new_.underline;
  }

  if (new_.underline2 != cur_.underline2) {
    PrintSep(&first);
    (*out_) << (new_.underline2 ? kUnderline2On : kUnderline2Off);
    cur_.underline2 = new_.underline2;
  }

  if (new_.strike != cur_.strike) {
    PrintSep(&first);
    (*out_) << (new_.strike ? kStrikeOn : kStrikeOff);
    cur_.strike = new_.strike;
  }

  if (new_.blink != cur_.blink) {
    PrintSep(&first);
    (*out_) << (new_.blink ? kBlinkOn : kBlinkOff);
    cur_.blink = new_.blink;
  }

  if (new_.flip != cur_.flip) {
    PrintSep(&first);
    (*out_) << (new_.flip ? kFlipOn : kFlipOff);
    cur_.flip = new_.flip;
  }

  if (new_.fg != cur_.fg) {
    PrintCode(new_.fg, &first);
    cur_.fg = new_.fg;
  }

  if (new_.bg != cur_.bg) {
    PrintCode(new_.bg, &first);
    cur_.bg = new_.bg;
  }

  (*out_) << kEscapeEnd;
}

void XtermPrinter::SetBold(bool bold) {
  if (bold != new_.bold) {
    new_.bold = bold;
    dirty_ = true;
  }
}

void XtermPrinter::SetItalic(bool italic) {
  if (italic != new_.italic) {
    new_.italic = italic;
    dirty_ = true;
  }
}

void XtermPrinter::SetUnderline(bool underline) {
  if (underline != new_.underline) {
    new_.underline = underline;
    dirty_ = true;
  }
}

void XtermPrinter::SetUnderline2(bool underline2) {
  if (underline2 != new_.underline2) {
    new_.underline2 = underline2;
    dirty_ = true;
  }
}

void XtermPrinter::SetStrike(bool strike) {
  if (strike != new_.strike) {
    new_.strike = strike;
    dirty_ = true;
  }
}

void XtermPrinter::SetBlink(bool blink) {
  if (blink != new_.blink) {
    new_.blink = blink;
    dirty_ = true;
  }
}

void XtermPrinter::SetFlip(bool flip) {
  if (flip != new_.flip) {
    new_.flip = flip;
    dirty_ = true;
  }
}

void XtermPrinter::SetForeground256(const Pixel& color) {
  SetForeground256(rgb_to_xterm256(color.Copy().Opacify(bg_)));
}

void XtermPrinter::SetForeground256(int code) {
  if (!bgprint_ && new_.flip && code == bg256_) {
    code = 0;
  } else {
    code |= kForeground256;
  }
  if (code != new_.fg) {
    new_.fg = code;
    dirty_ = true;
  }
}

void XtermPrinter::SetBackground256(const Pixel& color) {
  SetBackground256(rgb_to_xterm256(color.Copy().Opacify(bg_)));
}

void XtermPrinter::SetBackground256(int code) {
  if (!bgprint_ && !new_.flip && code == bg256_) {
    code = 0;
  } else {
    code |= kBackground256;
  }
  if (code != new_.bg) {
    new_.bg = code;
    dirty_ = true;
  }
}

void XtermPrinter::PrintSep(bool* first) const {
  if (*first) {
    *first = false;
  } else {
    (*out_) << kEscapeSep;
  }
}

void XtermPrinter::PrintCode(int code, bool* first) {
  int part;
  part = (code >> 16) & 0xff;
  if (part) {
    PrintSep(first);
    (*out_) << part;
  }
  part = (code >> 8) & 0xff;
  if (part) {
    PrintSep(first);
    (*out_) << part;
  }
  part = code & 0xff;
  if (part) {
    PrintSep(first);
    (*out_) << part;
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
