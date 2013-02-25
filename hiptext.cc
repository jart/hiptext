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
DEFINE_string(font, "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf",
              "The path to the font .ttf file to use.");
DEFINE_int32(font_index, 0, "Index of face to use inside font .ttf file.");
DEFINE_int32(font_size, 11, "The size of the font in points.");
DEFINE_int32(font_res, 300, "The font dots per inch resoltuion.");
DEFINE_bool(hinting, true, "Enable font hinting.");
DEFINE_bool(color, true, "Enable xterm color.");
DEFINE_string(bg, "black", "The background color of your terminal.");

int GetFontLoadFlags() {
  return FT_LOAD_RENDER | (FLAGS_hinting ? 0 : FT_LOAD_NO_HINTING);
}

void Pad(int amt, char ch) {
  for (int n = 0; n < amt; ++n) {
    cout << ch;
  }
}

Graphic LoadLetter(FT_Face face, wchar_t letter, Pixel color) {
  CHECK(FT_Load_Char(face, letter, GetFontLoadFlags()) == 0);
  FT_Bitmap* bitmap = &face->glyph->bitmap;
  CHECK(bitmap->pixel_mode == FT_PIXEL_MODE_GRAY);
  const int width = face->glyph->metrics.horiAdvance >> 6;
  const int height = face->glyph->metrics.vertAdvance >> 6;
  const int baseline = (face->height - face->glyph->metrics.horiBearingY) >> 6;
  const int offset_x = face->glyph->metrics.horiBearingX >> 6;
  const int offset_y = baseline;
  Graphic graphic(width, height);
  for (int y = 0; y < bitmap->rows; ++y) {
    for (int x = 0; x < bitmap->width; ++x) {
      uint8_t grey = bitmap->buffer[y * bitmap->width + x];
      if (grey) {
        Pixel& pixel = graphic.Get(x + offset_x, y + offset_y);
        pixel.set_red(color.red());
        pixel.set_green(color.green());
        pixel.set_blue(color.blue());
        pixel.set_alpha(Color256(grey));
      }
    }
  }
  return graphic;
}

wstring Xterm256Pixel(const Pixel& pixel, const Pixel& background) {
  int fg_code = rgb_to_xterm256(pixel);
  int bg_code = rgb_to_xterm256(pixel.Copy().Opacify(background));
  return (L"\x1b[38;5;" + std::to_wstring(fg_code) + L"m" +
          L"\x1b[48;5;" + std::to_wstring(bg_code) + L"m");
}

void PrintImage(Graphic graphic, const Pixel& background) {
  CharQuantizer char_quantizer(DecodeUTF8(FLAGS_chars), 256);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      Pixel pixel = graphic.Get(x, y);
      if (FLAGS_color) {
        cout << Xterm256Pixel(pixel, background);
      }
      cout << char_quantizer.Quantize(ColorTo256(pixel.grey()));
    }
    if (FLAGS_color) {
      cout << "\x1b[0m";
    }
    cout << "\n";
  }
}

void Dimensions(FT_Face face) {
  int max_width = 0;
  int max_rows = 0;
  for (int letter = ' '; letter <= '~'; ++letter) {
    CHECK(FT_Load_Char(face, letter, GetFontLoadFlags()) == 0);
    FT_Bitmap* bitmap = &face->glyph->bitmap;
    CHECK(bitmap->pixel_mode == FT_PIXEL_MODE_GRAY);
    printf("%c = %03d x %03d off %dx%d metrics=%ld-%ld %ld-%ld\n",
           letter, bitmap->width, bitmap->rows,
           face->glyph->bitmap_left,
           face->glyph->bitmap_top,
           face->glyph->metrics.horiBearingX >> 6,
           face->glyph->metrics.horiBearingY >> 6,
           face->glyph->metrics.horiAdvance >> 6,
           face->glyph->metrics.vertAdvance >> 6);
    if (bitmap->width > max_width)
      max_width = bitmap->width;
    if (bitmap->rows > max_rows)
      max_rows = bitmap->rows;
  }
  printf("max = %03d x %03d\n", max_width, max_rows);
  printf("ascender = %d\n", face->ascender >> 6);
  printf("descender = %d\n", face->descender >> 6);
  printf("height = %d\n", face->height >> 6);
  printf("bbox x=%ld-%ld y=%ld-%ld\n",
         face->bbox.xMin >> 6, face->bbox.xMax >> 6,
         face->bbox.yMin >> 6, face->bbox.yMax >> 6);
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
  CHECK(FT_Set_Char_Size(face, FLAGS_font_size << 6, 0, FLAGS_font_res,0) == 0);
  PrintImage(LoadPNG("balls.png").BilinearScale(120, 50), bg);
  PrintImage(LoadJPEG("obama.jpg").BilinearScale(150, 100), bg);
  PrintImage(LoadLetter(face, '@', Pixel::kBlack).Opacify(Pixel::kWhite), bg);
  PrintImage(LoadLetter(face, '@', Pixel::kBlack), Pixel::kWhite);
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
