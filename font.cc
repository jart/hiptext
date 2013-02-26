#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "graphic.h"
#include "pixel.h"

DEFINE_string(font, "DejaVuSansMono.ttf",
              "The path to the font .ttf file to use.");
DEFINE_int32(font_index, 0, "Index of face to use inside font .ttf file.");
DEFINE_int32(font_size, 11, "The size of the font in points.");
DEFINE_int32(font_dpi, 300, "Dots per inch for font rendering.");
DEFINE_bool(hinting, true, "Enable font hinting.");

static FT_Library library;
static FT_Face face;

static int GetFontLoadFlags() {
  return (FT_LOAD_RENDER | FT_LOAD_LINEAR_DESIGN |
          ((FLAGS_hinting) ? 0 : FT_LOAD_NO_HINTING));
}

void InitFont() {
  CHECK(FT_Init_FreeType(&library) == 0);
  CHECK(FT_New_Face(library, FLAGS_font.c_str(), FLAGS_font_index, &face) == 0);
  CHECK(FT_Set_Char_Size(face, FLAGS_font_size << 6, 0, FLAGS_font_dpi,0) == 0);
}

Graphic LoadLetter(wchar_t letter, const Pixel& fg, const Pixel& bg) {
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
