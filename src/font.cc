// hiptext - Image to Text Converter
// By Justine Tunney

#include "hiptext/font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "hiptext/graphic.h"
#include "hiptext/pixel.h"

DEFINE_string(font, "DejaVuSansMono.ttf",
              "The path to the font .ttf file to use.");
DEFINE_int32(font_index, 0, "Index of face to use inside font .ttf file.");
DEFINE_int32(font_size, 11, "The size of the font in points.");
DEFINE_int32(font_dpi, 300, "Dots per inch for font rendering.");
DEFINE_bool(hinting, false, "Enable font hinting.");

static FT_Library g_library;
static FT_Face g_face;

static int GetFontLoadFlags() {
  return (FT_LOAD_RENDER | FT_LOAD_LINEAR_DESIGN |
          ((FLAGS_hinting) ? 0 : FT_LOAD_NO_HINTING));
}

void InitFont() {
  CHECK_EQ(0, FT_Init_FreeType(&g_library));
  CHECK_EQ(0, FT_New_Face(
      g_library, FLAGS_font.c_str(), FLAGS_font_index, &g_face));
  CHECK_EQ(0, FT_Set_Char_Size(
      g_face, FLAGS_font_size << 6, 0, FLAGS_font_dpi, 0));
}

static int CalculateBaseline(double advance, double descender, double height) {
  return advance * descender / height + advance;
}

Graphic LoadLetter(wchar_t letter, const Pixel& fg, const Pixel& bg) {
  CHECK_EQ(0, FT_Load_Char(g_face, letter, GetFontLoadFlags()));
  FT_Bitmap* bitmap = &g_face->glyph->bitmap;
  FT_Glyph_Metrics* metrics = &g_face->glyph->metrics;
  CHECK_EQ(FT_PIXEL_MODE_GRAY, bitmap->pixel_mode);
  const int baseline = CalculateBaseline(
      metrics->vertAdvance, g_face->descender, g_face->height);
  const int width = metrics->horiAdvance >> 6;
  const int height = metrics->vertAdvance >> 6;
  const int offset_x = metrics->horiBearingX >> 6;
  const int offset_y = (baseline - metrics->horiBearingY) >> 6;
  Graphic graphic(width, height, bg);
  for (unsigned y = 0; y < bitmap->rows; ++y) {
    int y2 = y + offset_y;
    if (y2 < 0 || y2 >= height)
      continue;
    for (unsigned x = 0; x < bitmap->width; ++x) {
      uint8_t grey = bitmap->buffer[y * bitmap->width + x];
      if (grey) {
        int x2 = x + offset_x;
        if (x2 < 0 || x2 >= width)
          continue;
        double grey255 = grey / 255;
        graphic.Get(x2, y2).Overlay(fg.Copy().set_alpha(grey255));
      }
    }
  }
  return graphic;
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
