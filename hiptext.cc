// hiptext - Image to Text Converter
// Copyright (c) 2013 Justine Tunney

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <numeric>
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
#include "macterm.h"
#include "movie.h"
#include "pixel.h"
#include "png.h"
#include "unicode.h"
#include "xterm256.h"
#include "termprinter.h"

using std::cout;
using std::string;
using std::wstring;

DEFINE_string(chars, u8"\u00a0\u2591\u2592\u2593\u2588",
              "The quantization character array.");
DEFINE_bool(color, true, "Use --nocolor to disable color altogether.");
DEFINE_bool(macterm, false, "Optimize for Mac OS X Terminal.app.");
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

DEFINE_int32(width, 0, "Width of rendering. Defaults to 0, in which case it "
           "automatically detects the terminal width. If height is not "
           "provided, it still maintains the aspect ratio.");
DEFINE_int32(height, 0, "Height of rendering. Defaults to 0, in which case it "
           "automatically maintains the aspect ratio with respect to width.");

static const wchar_t kUpperHalfBlock = L'\u2580';
static const wchar_t kLowerHalfBlock = L'\u2584';
static const wchar_t kFullBlock = L'\u2588';

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
  for (int xterm_bg = 17; xterm_bg < 256; ++xterm_bg) {
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
  CHECK_NOTNULL(best);
  assert(best != nullptr);
  return *best;
}

winsize GetTerminalSize() {
  winsize ws;
  PCHECK(ioctl(0, TIOCGWINSZ, &ws) == 0);
  return ws;
}

void PrintImageXterm256(std::ostream& os, const Graphic& graphic) {
  TermPrinter out(os);
  Pixel bg = Pixel(FLAGS_bg);
  int bg256 = rgb_to_xterm256(bg);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      int code = rgb_to_xterm256(graphic.Get(x, y).Copy().Opacify(bg));
      if (!FLAGS_bgprint && code == bg256) {
        out.SetBackground256(0);
      } else {
        out.SetBackground256(code);
      }
      out << FLAGS_space;
    }
    out.Reset();
    out << "\n";
  }
}

void PrintImageXterm256Hack1(std::ostream& os, const Graphic& graphic) {
  TermPrinter out(os);
  Pixel bg = Pixel(FLAGS_bg);
  int bg256 = rgb_to_xterm256(bg);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      const Pixel& color = graphic.Get(x, y).Copy().Opacify(bg);
      const Combo& combo = QuantizeXterm256Hack1(color);
      if (combo.xterm_fg)
        out.SetForeground256(combo.xterm_fg);
      if (combo.xterm_bg)
        out.SetBackground256(combo.xterm_bg);
      if (!FLAGS_bgprint && (combo.xterm_fg == bg256 &&
                             combo.xterm_bg == bg256)) {
        out << FLAGS_space;
      } else {
        out << combo.ch;
      }
    }
    out.Reset();
    out << "\n";
  }
}

void PrintImageXterm256Hack2(std::ostream& os, const Graphic& graphic) {
  TermPrinter out(os);
  Pixel bg = Pixel(FLAGS_bg);
  int bg256 = rgb_to_xterm256(bg);
  int height = graphic.height() - graphic.height() % 2;
  for (int y = 0; y < height; y += 2) {
    for (int x = 0; x < graphic.width(); ++x) {
      const Pixel& top = graphic.Get(x, y);
      const Pixel& bottom = graphic.Get(x, y + 1);
      int top256 = rgb_to_xterm256(top);
      int bottom256 = rgb_to_xterm256(bottom);
      if (!FLAGS_bgprint && (top256 == bg256 &&
                             bottom256 == bg256)) {
        out.SetForeground256(0);
        out.SetBackground256(0);
        out << FLAGS_space;
      } else {
        out.SetForeground256(top256);
        out.SetBackground256(bottom256);
        out << kUpperHalfBlock;
      }
    }
    out.Reset();
    out << "\n";
  }
}

void PrintImageMacterm(std::ostream& os, const Graphic& graphic) {
  TermPrinter out(os);
  Pixel bg = Pixel(FLAGS_bg);
  int height = graphic.height() - graphic.height() % 2;
  for (int y = 0; y < height; y += 2) {
    for (int x = 0; x < graphic.width(); ++x) {
      MactermColor color(graphic.Get(x, y + 0).Copy().Opacify(bg),
                         graphic.Get(x, y + 1).Copy().Opacify(bg));
      out.SetForeground256(color.fg());
      out.SetBackground256(color.bg());
      out << color.symbol();
    }
    out.Reset();
    out << "\n";
  }
}

void PrintImageNoColor(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel(FLAGS_bg);
  wstring chars = DecodeText(FLAGS_chars);
  CharQuantizer quantizer(chars, 256);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      const Pixel& pixel = graphic.Get(x, y);
      if (bg == Pixel::kWhite) {
        os << quantizer.Quantize(255 - static_cast<int>(pixel.grey() * 255));
      } else {
        os << quantizer.Quantize(static_cast<int>(pixel.grey() * 255));
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
  // int term_height = GetTerminalSize().ws_row;
  // Default to aspect-ratio unless |height| gflag is provided.
  int width = std::min(graphic.width(), term_width);
  width = FLAGS_width ? FLAGS_width : width;
  int height = FLAGS_height ? FLAGS_height :
      AspectHeight(width, graphic.width(), graphic.height());

  if (FLAGS_color) {
    if (FLAGS_xterm256_hack1) {
      PrintImageXterm256Hack1(
          os, graphic.BilinearScale(width, height / 2));
    } else if (FLAGS_xterm256_hack2) {
      PrintImageXterm256Hack2(
          os, graphic.BilinearScale(width, height));
    } else if (FLAGS_macterm) {
      PrintImageMacterm(
          os, graphic.BilinearScale(width, height));
    } else {
      PrintImageXterm256(
          os, graphic.BilinearScale(width, height / 2));
    }
  } else {
    PrintImageNoColor(
        os, graphic.BilinearScale(width, height / 2));
  }
}

inline void HideCursor() {
  cout << "\x1b[?25l";
}

inline void ShowCursor() {
  cout << "\x1b[?25h";
}

inline void ResetCursor() {
  cout << "\x1b[H\n"; 
}

void PrintMovie(Movie movie) {
  // for (const auto& frame : movie) {
  while(1) {
    ResetCursor();
    PrintImage(cout, movie.Next());
  }
}

// Prints all the frames from a directory.
// Assumes mplayer generated all these from a .jpg
void PrintMovie(const string& dir, const int frames) {
  HideCursor();
  for (int frame = 1; frame <= frames; ++frame) {
    ResetCursor();
    std::stringstream ss;
    char buf[128];
    snprintf(buf, sizeof(buf), "%s/%08d.jpg", dir.data(), frame);
    PrintImage(ss, LoadJPEG(buf));
    cout << ss.str();
    // timespec req = {0, 500000000};
    // nanosleep(&req, NULL);
  }
  ShowCursor();
}

Graphic GenerateSpectrum(int width, int height) {
  int bar_width = static_cast<double>(width) * 0.05;
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

    // Render the red/white gradient bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(1.0, fy / height, fy / height);
    }

    // Render the green/white gradient bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(fy / height, 1.0, fy / height);
    }

    // Render the blue/white gradient bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(fy / height, fy / height, 1.0);
    }
  }
  return res;
}


inline string GetExtension(const string& path) {
  string s = path.substr(path.find_last_of('.') + 1);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

void sig_handler(int sig) {
  // Ensure that the terminal cursor is visible even after premature exit.
  cout << sig;
  ShowCursor();
  exit(1);
}

int main(int argc, char** argv) {
  // Command Validation
  if (argc < 2) {
    cout << "Must provide path to desired image/movie file.\n";
    exit(1);
  }
  string path = argv[1];
  // if (!isatty(1))
  //   FLAGS_color = false;
  google::SetUsageMessage("hiptext [FLAGS]");
  google::SetVersionString("0.1");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  const char* lang = std::getenv("LANG");
  if (lang == nullptr) lang = "en_US.utf8";
  std::locale::global(std::locale(lang));
  InitFont();
  if (FLAGS_xterm256_hack1)
    InitXterm256Hack1();

  signal(SIGINT, &sig_handler);
  struct ::stat dinfo;
  ::stat(path.data(), &dinfo);
  if (S_ISDIR(dinfo.st_mode)) {
    // If directory, print a movie using all the frames..
    cout << "Printing a Movie from directory.\n";
    PrintMovie(path, 1000);
    return 0;
  }
  // Otherwise, print a single image
  string extension = GetExtension(path);
  cout << "Hiptexting: " << argv[1] << ". File Type: " << extension << "\n";
  if (extension == "png") {    
    PrintImage(cout, LoadPNG(path));
  } else if (extension == "jpg" || extension == "jpeg") {    
    PrintImage(cout, LoadJPEG(path));
  } else if (extension == "mov") {
    Movie movie = Movie(path, FLAGS_width);
    PrintMovie(movie);
  } else {
    cout << "Unknown Filetype.\n";
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
