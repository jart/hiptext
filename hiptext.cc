#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <locale>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "charquantizer.h"
#include "font.h"
#include "graphic.h"
#include "jpeg.h"
#include "pixel.h"
#include "png.h"
#include "utf8.h"
#include "xterm256.h"
#include "xtermprinter.h"

using std::cout;
using std::string;
using std::wstring;

DEFINE_string(chars, u8"\u00a0\u2591\u2592\u2593\u2588",
              "The quantization character array.");
DEFINE_bool(color, true, "Use --nocolor to disable color altogether.");
DEFINE_bool(xterm256, true, "Enable xterm-256color output.");
DEFINE_bool(xterm256_hack1, false, "Enable xterm256 color hack #1.");
DEFINE_string(bg, "black", "The native background of your terminal specified "
              "as a CSS or X11 color value. If you're a real hacker this will "
              "be black, but some insane desktops like to coerce people into "
              "using white (or even purple!) terminal backgrounds by default. "
              "When using the --nocolor mode you should set this to white if "
              "you plan copy/pasting the output into something with a white "
              "background like if you were spamming Reddit.");
DEFINE_bool(bgprint, false, "Enable explicit styling when printing characters "
            "that are nearly identical to the native terminal background.");
DEFINE_string(space, u8"\u00a0", "The empty character to use when printing. "
              "By default this is a utf8 non-breaking space.");

struct Combo {
  Combo(Pixel color, wchar_t ch, uint8_t xterm_fg, uint8_t xterm_bg)
      : color(color), ch(ch), xterm_fg(xterm_fg), xterm_bg(xterm_bg) {}
  Pixel color;
  wchar_t ch;
  int xterm_fg;
  int xterm_bg;
};

std::vector<Combo> combos;

void PrintImageXterm256(const Graphic& graphic) {
  XtermPrinter out(&cout, Pixel::Parse(FLAGS_bg), FLAGS_bgprint);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      out.SetBackground256(graphic.Get(x, y));
      out << FLAGS_space;
    }
    out.Reset();
    out << "\n";
  }
}

void PrintImageXterm256Hack1(const Graphic& graphic) {
  Pixel bg = Pixel::Parse(FLAGS_bg);
  XtermPrinter out(&cout, bg, FLAGS_bgprint);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      Pixel color = graphic.Get(x, y).Copy().Opacify(bg);
      const Combo* best = nullptr;
      float best_dist = 1e6;
      for (const auto& combo : combos) {
        float dist = color.Distance(combo.color);
        if (dist < best_dist) {
          best = &combo;
          best_dist = dist;
        }
      }
      if (best->xterm_fg)
        out.SetForeground256(best->xterm_fg);
      if (best->xterm_bg)
        out.SetBackground256(best->xterm_bg);
      out << best->ch;
    }
    out.Reset();
    out << "\n";
  }
}

void PrintImageNoColor(const Graphic& graphic) {
  Pixel bg = Pixel::Parse(FLAGS_bg);
  wstring chars = DecodeUTF8(FLAGS_chars);
  CharQuantizer quantizer(chars, 256);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      Pixel pixel = graphic.Get(x, y);
      if (bg == Pixel::kWhite) {
        cout << quantizer.Quantize(255 - ColorTo256(pixel.grey()));
      } else {
        cout << quantizer.Quantize(ColorTo256(pixel.grey()));
      }
    }
    cout << "\n";
  }
}

void PrintImage(const Graphic& graphic) {
  if (FLAGS_color) {
    if (FLAGS_xterm256_hack1) {
      PrintImageXterm256Hack1(graphic);
    } else {
      PrintImageXterm256(graphic);
    }
  } else {
    PrintImageNoColor(graphic);
  }
}

void InitXterm256Hack1() {
  for (int xterm_bg = 16; xterm_bg <= 255; ++xterm_bg) {
    combos.emplace_back(xterm_to_rgb(xterm_bg), L' ', 0, xterm_bg);
  }
  for (int xterm_fg = 16; xterm_fg <= 255; ++xterm_fg) {
    for (int xterm_bg = 16; xterm_bg <= 255; ++xterm_bg) {
      Pixel color = xterm_to_rgb(xterm_bg);
      color.Overlay(xterm_to_rgb(xterm_fg).Copy().set_alpha(0.5));
      combos.emplace_back(color, L'\u2591', xterm_fg, xterm_bg);
    }
  }
}

// void LoadCombos(FT_Face face) {
//   for (auto ch : DecodeUTF8(FLAGS_chars)) {
//     for (int xterm_fg = 16; xterm_fg <= 255; ++xterm_fg) {
//       for (int xterm_bg = 16; xterm_bg <= 255; ++xterm_bg) {
//         Graphic letter = LoadLetter(ch, xterm_to_rgb(xterm_fg),
//                                     xterm_to_rgb(xterm_bg));
//         combos.emplace_back(letter.GetAverageColor(), ch, xterm_fg, xterm_bg);
//       }
//     }
//   }
// }

int main(int argc, char** argv) {
  if (!isatty(1))
    FLAGS_color = false;
  google::SetUsageMessage("hiptext [FLAGS]");
  google::SetVersionString("0.1");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  std::locale::global(std::locale("en_US.utf8"));
  InitFont();
  if (FLAGS_xterm256_hack1)
    InitXterm256Hack1();
  // XtermPrinter out(&cout, Pixel::Parse(FLAGS_bg), FLAGS_bgprint);
  // out.SetBold(true);
  // out << "hello\n";
  PrintImage(LoadPNG("balls.png").BilinearScale(120, 50));
  PrintImage(LoadJPEG("obama.jpg").BilinearScale(150, 100));
  // PrintImage(LoadLetter(L'a', Pixel::kWhite, Pixel::kClear));
  return 0;
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
