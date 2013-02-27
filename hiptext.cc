#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <algorithm>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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
DEFINE_bool(xterm256_hack2, false, "Enable xterm256 color hack #2.");
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

const wchar_t kUpperHalfBlock = L'\u2580';

struct Combo {
  Combo(Pixel color, wchar_t ch, uint8_t xterm_fg, uint8_t xterm_bg)
      : color(color), ch(ch), xterm_fg(xterm_fg), xterm_bg(xterm_bg) {}
  Pixel color;
  wchar_t ch;
  int xterm_fg;
  int xterm_bg;
};

// std::vector<int> g_combos_red;
// std::vector<int> g_combos_green;
// std::vector<int> g_combos_blue;
// std::unordered_map<int, Combo> g_combos;
std::vector<Combo> g_combos;

void InitXterm256Hack1() {
  for (int xterm_bg = 16; xterm_bg < 256; ++xterm_bg) {
    g_combos.emplace_back(xterm_to_rgb(xterm_bg), L' ', 0, xterm_bg);
  }
  for (int xterm_fg = 17; xterm_fg < 232; ++xterm_fg) {
    for (int xterm_bg = 17; xterm_bg < 232; ++xterm_bg) {
      Pixel bg = xterm_to_rgb(xterm_bg);
      Pixel fg = xterm_to_rgb(xterm_fg);
      bg.MixKubelkaMunk(fg);
      g_combos.emplace_back(bg, L'\u2591', xterm_fg, xterm_bg);
    }
  }
}

const Combo& QuantizeXterm256Hack1(const Pixel& color) {
  const Combo* best = nullptr;
  double best_dist = 1e6;
  for (const auto& combo : g_combos) {
    double dist = color.Distance(combo.color);
    if (dist < best_dist) {
      best = &combo;
      best_dist = dist;
    }
  }
  DCHECK(best != nullptr);
  return *best;
}

winsize GetTerminalSize() {
  winsize ws;
  PCHECK(ioctl(0, TIOCGWINSZ, &ws) == 0);
  return ws;
}

void PrintImageXterm256(std::ostream& os, const Graphic& graphic) {
  XtermPrinter out(&os, Pixel::Parse(FLAGS_bg), FLAGS_bgprint);
  bool first = true;
  for (int y = 0; y < graphic.height(); ++y) {
    if (first) {
      first = false;
    } else {
      out << "\n";
    }
    for (int x = 0; x < graphic.width(); ++x) {
      out.SetBackground256(graphic.Get(x, y));
      out << FLAGS_space;
    }
    out.Reset();
  }
}

void PrintImageXterm256Hack1(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel::Parse(FLAGS_bg);
  XtermPrinter out(&os, bg, FLAGS_bgprint);
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

void PrintImageXterm256Hack2(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel::Parse(FLAGS_bg);
  XtermPrinter out(&os, bg, FLAGS_bgprint);
  int height = graphic.height() - graphic.height() % 2;
  for (int y = 0; y < height; y += 2) {
    for (int x = 0; x < graphic.width(); ++x) {
      bool same_as_bg = true;
      same_as_bg &= out.SetForeground256(graphic.Get(x, y));
      same_as_bg &= out.SetBackground256(graphic.Get(x, y + 1));
      if (same_as_bg) {
        out << FLAGS_space;
      } else {
        out << kUpperHalfBlock;
      }
    }
    out.Reset();
    out << "\n";
  }
}

void PrintImageNoColor(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel::Parse(FLAGS_bg);
  wstring chars = DecodeUTF8(FLAGS_chars);
  CharQuantizer quantizer(chars, 256);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      Pixel pixel = graphic.Get(x, y);
      if (bg == Pixel::kWhite) {
        os << quantizer.Quantize(255 - ColorTo256(pixel.grey()));
      } else {
        os << quantizer.Quantize(ColorTo256(pixel.grey()));
      }
    }
    cout << "\n";
  }
}

static int AspectHeight(double new_width, double width, double height) {
  return new_width / width * height;
}

void PrintImage(std::ostream& os, const Graphic& graphic) {
  int term_width = GetTerminalSize().ws_col;
  int term_height = GetTerminalSize().ws_row;
  int width = std::min(graphic.width(), term_width);
  if (FLAGS_color) {
    if (FLAGS_xterm256_hack1) {
      PrintImageXterm256Hack1(
          os, graphic.BilinearScale(width, AspectHeight(
              width, graphic.width(), graphic.height()) / 2));
    } else if (FLAGS_xterm256_hack2) {
      PrintImageXterm256Hack2(
          os, graphic.BilinearScale(width, AspectHeight(
              width, graphic.width(), graphic.height())));
    } else {
      PrintImageXterm256(
          os, graphic.BilinearScale(term_width, term_height));
      // PrintImageXterm256(
      //     os, graphic.BilinearScale(width, AspectHeight(
      //         width, graphic.width(), graphic.height()) / 2));
    }
  } else {
    PrintImageNoColor(
        os, graphic.BilinearScale(width, AspectHeight(
            width, graphic.width(), graphic.height()) / 2));
  }
}

Graphic GenerateSpectrum(int width, int height) {
  int bar_width = (double)width * 0.05;
  int spec_width = width - bar_width * 4;
  double hh = static_cast<double>(height) / 2.0;
  Graphic res(width, height);
  for (int y = 0; y < height; ++y) {
    double fy = static_cast<double>(y);

    // Render the large color spectrum.
    for (int x = 0; x < spec_width; ++x) {
      double fx = static_cast<double>(x);
      res.Get(x, y) = Pixel(
          fx / spec_width,
          (y > hh) ? 1.0 : fy / (hh),
          (y < hh) ? 1.0 : 1.0 - (fy - (hh)) / (hh)).FromHSV();
    }

    // Render the grey bar.
    int offset = spec_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) =
          Pixel(0.0, 0.0, fy / height).FromHSV();
    }

    // Render the red bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(1.0, fy / height, fy / height);
    }

    // Render the green bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(fy / height, 1.0, fy / height);
    }

    // Render the blue bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(fy / height, fy / height, 1.0);
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

  // Graphic lol(256, 3);
  // for (int fg = 17; fg < 232; ++fg) {
  //   Pixel pix = xterm_to_rgb(fg);
  //   lol.Get(pix.red() * 255, 0) = Pixel::kBlack;
  //   lol.Get(pix.green() * 255, 1) = Pixel::kBlack;
  //   lol.Get(pix.blue() * 255, 2) = Pixel::kBlack;
  // }
  // WritePNG(lol, "/home/jart/www/graphic.png");

  // Graphic lol(256, 256, Pixel::Parse("grey"));
  // for (int fg = 16; fg < 256; ++fg) {
  //   Pixel pix = xterm_to_rgb(fg);
  //   cout << fg << " = " << pix << "\n";
  //   lol.Get(pix.red() * 255,
  //           pix.blue() * 255) = Pixel(0, 0, pix.blue());
  // }
  // WritePNG(lol, "/home/jart/www/graphic.png");

  // InitXterm256Hack1();
  // Graphic lol(256, 256, Pixel::Parse("grey"));
  // int n = 0;
  // for (const auto& combo : g_combos) {
  //   n++;
  //   Pixel pix = combo.color;
  //   CHECK(pix.red() >= 0) << n;
  //   lol.Get(pix.red() * 255,
  //           pix.green() * 255) = Pixel(0, 0, pix.blue());
  // }
  // WritePNG(lol, "/home/jart/www/graphic.png");

  // XtermPrinter out(&cout, Pixel::Parse(FLAGS_bg), FLAGS_bgprint);
  // out.SetBold(true);
  // out << "hello\n";
  // PrintImage(cout, LoadPNG("balls.png"));
  // PrintImage(cout, LoadJPEG("obama.jpg"));
  // PrintImage(cout, LoadLetter(L'@', Pixel::kWhite, Pixel::kClear));

  // InitXterm256Hack1();
  // Graphic spectrum = GenerateSpectrum(200, 100);
  // for (int y = 0; y < spectrum.height(); ++y) {
  //   for (int x = 0; x < spectrum.width(); ++x) {
  //     spectrum.Get(x, y) = QuantizeXterm256Hack1(spectrum.Get(x, y)).color;
  //     // spectrum.Get(x, y) = xterm_to_rgb(rgb_to_xterm256(spectrum.Get(x, y)));
  //   }
  // }
  // WritePNG(spectrum, "/home/jart/www/graphic.png");

  // XtermPrinter out(&cout, Pixel::kBlack, false);
  // out.SetBackground256(Pixel::kGreen);
  // out.SetForeground256(Pixel::kGreen);
  // out << L'\u2580';
  // out << L'\u2580';
  // out << L'\u2580';
  // out << L'\u2580';
  // out << L'\n';

  cout << "\x1b[?25l";  // Hide cursor.
  for (int frame = 1; frame <= 1000; ++frame) {
    std::stringstream ss;
    ss << "\n";
    char buf[128];
    snprintf(buf, sizeof(buf), "rickroll/%08d.jpg", frame);
    PrintImage(ss, LoadJPEG(buf));
    cout << ss.str();
    timespec req = {0, 50000000};
    nanosleep(&req, NULL);
  }

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
