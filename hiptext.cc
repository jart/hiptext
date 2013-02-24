#include <cstdio>
#include <iostream>
#include <locale>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphic.h"
#include "pixel.h"
#include "png.h"
#include "utf8.h"
#include "xterm256.h"

using std::cout;
using std::string;
using std::wstring;

DEFINE_string(chars, u8" \u2591\u2592\u2593\u2588",
              "The quantization character array.");
DEFINE_string(font, "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf",
              "The path to the font .ttf file to use.");
DEFINE_int32(font_index, 0, "Index of face to use inside font .ttf file.");
DEFINE_int32(font_width, 64, "The nominal font char width in pixels.");
DEFINE_int32(font_height, 64, "The nominal font char height in pixels.");
DEFINE_bool(hinting, false, "Enable font hinting.");

int GetFontLoadFlags() {
  return FT_LOAD_RENDER | (FLAGS_hinting ? 0 : FT_LOAD_NO_HINTING);
}

class CharQuantizer {
 public:
  CharQuantizer(const wstring& chars, int size) : map_(size) {
    const int segment_size = size / chars.size() + 1;
    for (int n = 0; n < size; ++n) {
      map_[n] = chars[n / segment_size];
    }
  }

  inline wchar_t Quantize(int color) const {
    DCHECK(0 <= color && color < map_.size());
    return map_[color];
  }

  CharQuantizer(const CharQuantizer& other) = delete;
  void operator=(const CharQuantizer& other) = delete;

 private:
  std::vector<wchar_t> map_;
};

void PrintLetter(wchar_t letter) {
  FT_Library library;
  FT_Face face;
  CHECK(FT_Init_FreeType(&library) == 0);
  CHECK(FT_New_Face(library, FLAGS_font.c_str(), FLAGS_font_index, &face) == 0);
  CHECK(FT_Set_Pixel_Sizes(face, FLAGS_font_width, FLAGS_font_height) == 0);
  CHECK(FT_Load_Char(face, letter, GetFontLoadFlags()) == 0);
  CharQuantizer char_quantizer(DecodeUTF8(FLAGS_chars), 256);
  FT_Bitmap* bitmap = &face->glyph->bitmap;
  CHECK(bitmap->pixel_mode == FT_PIXEL_MODE_GRAY);
  for (int y = 0; y < bitmap->rows; ++y) {
    for (int x = 0; x < bitmap->width; ++x) {
      int pixel = bitmap->buffer[y * bitmap->width + x];
      cout << char_quantizer.Quantize(pixel);
    }
    cout << "\n";
  }
}

void PrintImage(const string& path) {
  Graphic png = LoadPNG(path).BilinearScale(120, 50);
  CharQuantizer char_quantizer(DecodeUTF8(FLAGS_chars), 256);
  for (int y = 0; y < png.height(); ++y) {
    for (int x = 0; x < png.width(); ++x) {
      Pixel pix = png.Get(x, y);
      cout << "\x1b[";
      if (pix.alpha() > 0.0) {
        cout << "38;5;" << (int)rgb_to_xterm256(pix);
      } else {
        cout << "0";
      }
      cout << "m";
      cout << char_quantizer.Quantize((int)(pix.grey() * pix.alpha() * 255.0));
    }
    cout << "\x1b[0m\n";
  }
}

int main(int argc, char** argv) {
  google::SetUsageMessage("hiptext [FLAGS]");
  google::SetVersionString("0.1");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  std::locale::global(std::locale("en_US.utf8"));
  // PrintLetter('a');
  PrintImage("/home/jart/balls.png");
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
