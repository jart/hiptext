#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <locale>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "charquantizer.h"
#include "graphic.h"
#include "jpeg.h"
#include "pixel.h"
#include "png.h"
#include "utf8.h"
#include "xterm256.h"

using std::cout;
using std::string;
using std::wstring;

DEFINE_string(chars, u8"\u00A0\u2591\u2592\u2593\u2588",
              "The quantization character array.");
DEFINE_string(font, "DejaVuSansMono.ttf",
              "The path to the font .ttf file to use.");
DEFINE_int32(font_index, 0, "Index of face to use inside font .ttf file.");
DEFINE_int32(font_size, 11, "The size of the font in points.");
DEFINE_int32(font_dpi, 100, "Dots per inch for font rendering.");
DEFINE_bool(hinting, true, "Enable font hinting.");
DEFINE_bool(color, true, "Enable xterm 256 color.");
DEFINE_bool(supercolor, true, "Enable xterm beyond 256 color hack.");
DEFINE_string(bg, "black", "The background of your terminal.");

struct Combo {
  Combo(Pixel color, wchar_t ch, uint8_t xterm_fg, uint8_t xterm_bg)
      : color(color), ch(ch), xterm_fg(xterm_fg), xterm_bg(xterm_bg) {}
  Pixel color;
  wchar_t ch;
  uint8_t xterm_fg;
  uint8_t xterm_bg;
};

std::vector<Combo> combos;

int GetFontLoadFlags() {
  return (FT_LOAD_RENDER | FT_LOAD_LINEAR_DESIGN |
          ((FLAGS_hinting) ? 0 : FT_LOAD_NO_HINTING));
}

Graphic LoadLetter(FT_Face face, wchar_t letter, const Pixel& fg,
                   const Pixel& bg) {
  CHECK(FT_Load_Char(face, letter, GetFontLoadFlags()) == 0);
  FT_Bitmap* bitmap = &face->glyph->bitmap;
  FT_Glyph_Metrics* metrics = &face->glyph->metrics;
  CHECK(bitmap->pixel_mode == FT_PIXEL_MODE_GRAY);
  const int baseline = (int)(
      (double)metrics->vertAdvance + (double)metrics->vertAdvance *
      (double)face->descender / (double)face->height);
  const int width = metrics->horiAdvance >> 6;
  const int height = metrics->vertAdvance >> 6;
  const int offset_x = metrics->horiBearingX >> 6;
  const int offset_y = (baseline - metrics->horiBearingY) >> 6;
  Graphic graphic(width, height, bg);
  for (int y = 0; y < bitmap->rows; ++y) {
    int y2 = y + offset_y;
    if (y2 < 0 || y2 >= graphic.height())
      continue;
    for (int x = 0; x < bitmap->width; ++x) {
      uint8_t grey = bitmap->buffer[y * bitmap->width + x];
      if (grey) {
        int x2 = x + offset_x;
        if (x2 < 0 || x2 >= graphic.width())
          continue;
        graphic.Get(x2, y2).Overlay(fg.Copy().set_alpha(Color256(grey)));
      }
    }
  }
  return graphic;
}

void PrintImage(Graphic graphic, const Pixel& bg) {
  CharQuantizer char_quantizer(DecodeUTF8(FLAGS_chars), 256);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      Pixel pixel = graphic.Get(x, y);
      if (FLAGS_color) {
        int fg_code = rgb_to_xterm256(pixel);
        int bg_code = rgb_to_xterm256(pixel.Copy().Opacify(bg));
        cout << L"\x1b[38;5;" + std::to_wstring(fg_code) + L"m"
             << L"\x1b[48;5;" + std::to_wstring(bg_code) + L"m"
             << char_quantizer.Quantize(ColorTo256(pixel.grey()))
             << "\x1b[0m";
      } else {
        cout << char_quantizer.Quantize(ColorTo256(pixel.grey()));
      }
    }
    cout << "\n";
  }
}

void LoadCombos(FT_Face face) {
  for (auto ch : DecodeUTF8(FLAGS_chars)) {
    for (uint8_t xterm_fg = 16; xterm_fg <= 255; ++xterm_fg) {
      for (uint8_t xterm_bg = 16; xterm_bg <= 255; ++xterm_bg) {
        Graphic letter = LoadLetter(face, ch, xterm_to_rgb(xterm_fg),
                                    xterm_to_rgb(xterm_bg));
        combos.emplace_back(letter.GetAverageColor(), ch, xterm_fg, xterm_bg);
      }
    }
  }
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
  FT_Library library;
  FT_Face face;
  CHECK(FT_Init_FreeType(&library) == 0);
  Pixel bg = Pixel::Parse(FLAGS_bg);
  CHECK(FT_New_Face(library, FLAGS_font.c_str(), FLAGS_font_index, &face) == 0);
  CHECK(FT_Set_Char_Size(face, FLAGS_font_size << 6, 0, FLAGS_font_dpi,0) == 0);
  // LoadCombos(face);
  PrintImage(LoadPNG("balls.png").BilinearScale(120, 50), bg);
  PrintImage(LoadJPEG("obama.jpg").BilinearScale(150, 100), bg);
  PrintImage(LoadLetter(face, L'a', Pixel::kWhite, Pixel::kClear), bg);
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
