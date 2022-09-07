// hiptext - Image to Text Converter
// By Justine Tunney

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

#include "hiptext/artiste.h"
#include "hiptext/charquantizer.h"
#include "hiptext/font.h"
#include "hiptext/jpeg.h"
#include "hiptext/pixel.h"
#include "hiptext/png.h"
#include "hiptext/macterm.h"
#include "hiptext/movie.h"
#include "hiptext/xterm256.h"
#include "hiptext/termprinter.h"
#include "hiptext/sixelprinter.h"
#include "hiptext/unicode.h"

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
DEFINE_bool(sixel256, false, "Use sixel graphics (256 colors)");
DEFINE_bool(sixel16, false, "Use sixel graphics (16 colors)");
DEFINE_bool(sixel2, false, "Use sixel graphics (2 colors)");

static const wchar_t kUpperHalfBlock = L'\u2580';

// 256 color SIXEL is supported by RLogin, mlterm(X11/fb), and tanasinn.
// xterm with the option "-ti vt340" is limited up to 16 colors.
void PrintImageSixel256(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel(FLAGS_bg);
  SixelPrinter out(os, 256, false, FLAGS_bgprint, rgb_to_xterm256(bg));
  int width = graphic.width();
  int height = graphic.height();
  uint8_t (*to_index)(const Pixel&) = rgb_to_xterm256;

  out.Start();
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int code = to_index(graphic.Get(x, y).Copy().Opacify(bg));
      out.PrintPixel(code);
    }
    out.LineFeed();
  }
  out.End();
}

// 16 color SIXEL is supported by xterm with the option "-ti vt340"
void PrintImageSixel16(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel(FLAGS_bg);
  SixelPrinter out(os, 16, false, FLAGS_bgprint, rgb_to_xterm16(bg));
  int width = graphic.width();
  int height = graphic.height();
  uint8_t (*to_index)(const Pixel&) = rgb_to_xterm16;

  out.Start();
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int code = to_index(graphic.Get(x, y).Copy().Opacify(bg));
      out.PrintPixel(code);
    }
    out.LineFeed();
  }
  out.End();
}

void PrintImageSixel2(std::ostream& os, const Graphic& graphic) {
  Pixel bg = Pixel(FLAGS_bg);
  SixelPrinter out(os, 2, false, false, 0);
  int width = graphic.width();
  int height = graphic.height();
  uint8_t (*to_index)(const Pixel&) = rgb_to_xterm16;

  out.Start();
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int code = to_index(graphic.Get(x, y).Copy().Opacify(bg));
      out.PrintPixel(code);
    }
    out.LineFeed();
  }
  out.End();
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
    } else if (FLAGS_sixel2) {
      algo = PrintImageSixel2;
      duo_pixel = true;
    } else if (FLAGS_sixel16) {
      algo = PrintImageSixel16;
      duo_pixel = true;
    } else if (FLAGS_sixel256) {
      algo = PrintImageSixel256;
      duo_pixel = true;
    } else {
      algo = PrintImageXterm256;
    }
  } else {
    algo = PrintImageNoColor;
  }
  Artiste artiste(std::cout, std::cin, algo, duo_pixel,
                  FLAGS_sixel2 || FLAGS_sixel16 || FLAGS_sixel256);

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
  string uri_sig = "://"; // Given this string signature, assume that it's a video stream
  if (extension == "png") {
    artiste.PrintImage(LoadPNG(path));
  } else if (extension == "jpg" || extension == "jpeg") {
    artiste.PrintImage(LoadJPEG(path));
  } else if (extension == "mov" || extension == "mp4" || extension == "flv" ||
             extension == "avi" || extension == "mkv" || path.find(uri_sig) != string::npos) {
    artiste.PrintMovie(Movie(path));
  } else {
    fprintf(stderr, "Unknown Filetype: %s\n", extension.data());
    exit(1);
  }

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
