#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
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

void GetTerminalSize(int* width, int* height) {
  winsize ws;
  PCHECK(ioctl(0, TIOCGWINSZ, &ws) == 0);
  *width = ws.ws_col;
  *height = ws.ws_row;
}

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

double R(double a) {
  // solve a = (1 - c)^2 / (2c), c
  return a - std::sqrt(a * (a + 2)) + 1.0;
  // return (1.0 + a - std::sqrt(std::pow(a, 2.0) + (2.0 * a)));
}

double A(double c) {
  // This is K/S part of the the equations on that website.
  // a = (1 - c)^2 / (2c)
  return std::pow(1.0 - c, 2.0) / (2.0 * c);
}

void InitXterm256Hack1() {
  for (int xterm_bg = 16; xterm_bg < 256; ++xterm_bg) {
    combos.emplace_back(xterm_to_rgb(xterm_bg), L' ', 0, xterm_bg);
  }
  for (int xterm_fg = 17; xterm_fg < 232; ++xterm_fg) {
    for (int xterm_bg = 17; xterm_bg < 232; ++xterm_bg) {
      Pixel bg = xterm_to_rgb(xterm_bg);
      Pixel fg = xterm_to_rgb(xterm_fg);
      bg.set_red(R(A(fg.red()) + A(bg.red()) / 2));
      bg.set_green(R(A(fg.green()) + A(bg.green()) / 2));
      bg.set_blue(R(A(fg.blue()) + A(bg.blue()) / 2));
      // bg.ToKubelkaMunk();
      // fg.ToKubelkaMunk();
      // bg.Mix(fg);
      // bg.FromKubelkaMunk();
      combos.emplace_back(bg, L'\u2591', xterm_fg, xterm_bg);
    }
  }
}

const Combo& QuantizeXterm256Hack1(const Pixel& color) {
  const Combo* best = nullptr;
  float best_dist = 1e6;
  for (const auto& combo : combos) {
    float dist = color.Distance(combo.color);
    if (dist < best_dist) {
      best = &combo;
      best_dist = dist;
    }
  }
  DCHECK(best != nullptr);
  return *best;
}

void PrintImageXterm256Hack1(const Graphic& graphic) {
  Pixel bg = Pixel::Parse(FLAGS_bg);
  XtermPrinter out(&cout, bg, FLAGS_bgprint);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      Pixel color = graphic.Get(x, y).Copy().Opacify(bg);
      const Combo& combo = QuantizeXterm256Hack1(color);
      if (combo.xterm_fg)
        out.SetForeground256(combo.xterm_fg);
      if (combo.xterm_bg)
        out.SetBackground256(combo.xterm_bg);
      out << combo.ch;
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

Graphic GenerateSpectrum(int width, int height) {
  int grey_width = width / 10;
  int spec_width = width - grey_width;
  float hh = static_cast<float>(height) / 2.0;
  Graphic res(width, height);
  for (int y = 0; y < height; ++y) {
    float fy = static_cast<float>(y);
    for (int x = 0; x < spec_width; ++x) {
      float fx = static_cast<float>(x);
      res.Get(x, y) = Pixel(
          fx / spec_width,
          (y > hh) ? 1.0 : fy / (hh),
          (y < hh) ? 1.0 : 1.0 - (fy - (hh)) / (hh)).FromHSV().ToHSV().FromHSV();
    }
    for (int x = 0; x < grey_width; ++x) {
      res.Get(x + width - grey_width, y) =
          Pixel(0.0, 0.0, 1.0 - fy / height).FromHSV();
    }
  }
  return res;
}

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
  // PrintImage(LoadPNG("balls.png").BilinearScale(120, 50));
  // PrintImage(LoadJPEG("obama.jpg").BilinearScale(100, 60));
  // PrintImage(LoadLetter(L'a', Pixel::kWhite, Pixel::kClear));

  InitXterm256Hack1();
  Graphic spectrum = GenerateSpectrum(200, 100);
  for (int y = 0; y < spectrum.height(); ++y) {
    for (int x = 0; x < spectrum.width(); ++x) {
      spectrum.Get(x, y) = QuantizeXterm256Hack1(spectrum.Get(x, y)).color;
      // spectrum.Get(x, y) = xterm_to_rgb(rgb_to_xterm256(spectrum.Get(x, y)));
    }
  }
  WritePNG(spectrum, "/home/jart/www/graphic.png");

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
