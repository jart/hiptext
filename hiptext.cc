// hiptext - Image to Text Converter
// By Justine Tunney

// #include <unistd.h>
// #include <sys/stat.h>
// #include <sys/types.h>
#include <signal.h>
// #include <thread>

// #include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <string>
#include <sstream>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "artiste.h"
#include "charquantizer.h"
#include "font.h"
#include "jpeg.h"
#include "pixel.h"
#include "png.h"
#include "macterm.h"
#include "movie.h"
#include "xterm256.h"
#include "termprinter.h"
#include "unicode.h"

using std::cout;
using std::string;
using std::wstring;

DEFINE_string(chars, u8"\u00a0\u2591\u2592\u2593\u2588",
              "The quantization character array");
DEFINE_bool(color, true, "Use --nocolor to disable color altogether");
DEFINE_bool(macterm, false, "Optimize for Mac OS X Terminal.app");
DEFINE_bool(xterm256, true, "Enable xterm-256color output");
DEFINE_bool(xterm256unicode, false, "Enable xterm256 double-pixel hack");
DEFINE_string(bg, "black", "The native background of your terminal specified "
              "as a CSS or X11 color value. If you're a real hacker this will "
              "be black, but some insane desktops like to coerce people into "
              "using white (or even purple!) terminal backgrounds by default. "
              "When using the --nocolor mode you should set this to white if "
              "you plan copy/pasting the output into something with a white "
              "background like if you were spamming Reddit");
DEFINE_bool(bgprint, false, "Enable explicit styling when printing characters "
            "that are nearly identical to the native terminal background");
DEFINE_string(space, u8"\u00a0", "The empty character to use when printing. "
              "By default this is a utf8 non-breaking space");
DEFINE_bool(spectrum, false, "Show color spectrum graph");
DEFINE_bool(sixel, false, "Use sixel graphics (256 color)");

static const wchar_t kUpperHalfBlock = L'\u2580';
static const wchar_t kLowerHalfBlock = L'\u2584';
static const wchar_t kFullBlock = L'\u2588';

volatile bool g_done = false;

// 256 color SIXEL is supported by RLogin, mlterm(X11/fb), and tanasinn.
// xterm with the option "-ti vt340" is limited up to 16 colors.
void PrintImageSixel256(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel(FLAGS_bg);
  int bg256 = rgb_to_xterm256(bg);
  int cache;
  int count;
  int width = graphic.width();
  int height = graphic.height();
  char c;
  char slots[256];
  memset(slots, 0, sizeof(slots));

  os << "\033P0;0;8q\"1;1";

  for (int y = 0; y < height; ++y) {
    cache = 256;
    count = 1;
    for (int x = 0; x < width; ++x) {
      int code = rgb_to_xterm256(graphic.Get(x, y).Copy().Opacify(bg));
      if (count < 255 && cache == code) {
        ++count;
      } else {
        if (!FLAGS_bgprint && cache == bg256) {
          c = 0x3f;
        } else {
          c = (char)(0x3f + (1 << (y % 6)));
        }
        if (!slots[cache]) {
          const Pixel *pix = g_xterm + cache;

          // emit palette definition
          os << '#' << cache << ";2;"
             << static_cast<int>(pix->red() * 100)
             << ';'
             << static_cast<int>(pix->green() * 100)
             << ';'
             << static_cast<int>(pix->blue() * 100)
             ;
          slots[cache] = 1; // set dirty
        }
        os << "#" << cache;
        cache = code;
        if (count > 2) {
          os << '!' << count;
        } else if (count == 2) {
          os << c;
        }
        os << c;
        count = 1;
      }
    }
    if (count > 1) {
      if (!FLAGS_bgprint && cache == bg256) {
        c = 0x3f;
      } else {
        c = (char)(0x3f + (1 << (y % 6)));
      }
      if (count > 2) {
        os << "!" << count;
      } else if (count == 2) {
        os << c;
      }
      os << c;
    }
    os << "$";
    if (y % 6 == 5) {
      os << "-";
    }
  }
  os << "\033\\";
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

void PrintImageXterm256Unicode(std::ostream& os, const Graphic& graphic) {
  TermPrinter out(os);
  int height = graphic.height() - graphic.height() % 2;
  for (int y = 0; y < height; y += 2) {
    for (int x = 0; x < graphic.width(); ++x) {
      const Pixel& top = graphic.Get(x, y);
      const Pixel& bottom = graphic.Get(x, y + 1);
      int top256 = rgb_to_xterm256(top);
      int bottom256 = rgb_to_xterm256(bottom);
      out.SetForeground256(top256);
      out.SetBackground256(bottom256);
      out << kUpperHalfBlock;
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

void Sleep(int ms) {
  timespec req = {ms / 1000, ms % 1000 * 1000000};
  nanosleep(&req, nullptr);
}

inline string GetExtension(const string& path) {
  string s = path.substr(path.find_last_of('.') + 1);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

void OnCtrlC(int /*signal*/) {
  g_done = true;
}

int main(int argc, char** argv) {
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
  signal(SIGINT, OnCtrlC);
  InitFont();
  Movie::InitializeMain();

  RenderAlgorithm algo;
  bool duo_pixel = false;
  if (FLAGS_color) {
    if (FLAGS_xterm256unicode) {
      algo = PrintImageXterm256Unicode;
      duo_pixel = true;
    } else if (FLAGS_macterm) {
      algo = PrintImageMacterm;
      duo_pixel = true;
    } else if (FLAGS_sixel) {
      algo = PrintImageSixel256;
      duo_pixel = true;
    } else {
      algo = PrintImageXterm256;
    }
  } else {
    algo = PrintImageNoColor;
  }
  Artiste artiste(std::cout, std::cin, algo, duo_pixel, FLAGS_sixel);

  // Did they specify an option that requires no args?
  if (FLAGS_spectrum) {
    artiste.GenerateSpectrum();
    exit(0);
  }

  // Otherwise get an arg.
  if (argc < 2) {
    fprintf(stderr, "Missing file argument.\n"
            "Usage: %s [OPTIONS] [IMAGE_FILE | MOVIE_FILE]\n"
            "       %s --help\n", argv[0], argv[0]);
    exit(1);
  }
  string path = argv[1];
  string extension = GetExtension(path);
  if (extension == "png") {
    artiste.PrintImage(LoadPNG(path));
  } else if (extension == "jpg" || extension == "jpeg") {
    artiste.PrintImage(LoadJPEG(path));
  } else if (extension == "mov" || extension == "mp4" || extension == "flv" ||
             extension == "avi" || extension == "mkv") {
    artiste.PrintMovie(Movie(path));
  } else {
    fprintf(stderr, "Unknown Filetype: %s\n", extension.data());
    exit(1);
  }

  // std::thread t(Render, path);
  // t.join();
  exit(0);
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
