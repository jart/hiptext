#include "png.h"

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

#include <glog/logging.h>

#include "pixel.h"
#include "graphic.h"

Graphic LoadPNG(const std::string& path) {
  png_struct* png_;
  png_info* info_;
  uint8_t** rows;
  uint8_t header[8];
  FILE* fp = fopen(path.c_str(), "rb");
  PCHECK(fp) << path;
  PCHECK(fread(header, 8, 1, fp) == 1) << path;
  CHECK(png_sig_cmp(header, 0, 8) == 0) << "bad png file: " << path;
  png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info_ = png_create_info_struct(png_);
  CHECK(setjmp(png_jmpbuf(png_)) == 0) << "png read error: " << path;
  png_init_io(png_, fp);
  png_set_sig_bytes(png_, 8);
  png_read_info(png_, info_);
  int width = png_get_image_width(png_, info_);
  int height = png_get_image_height(png_, info_);
  int type = png_get_color_type(png_, info_);
  CHECK(type == PNG_COLOR_TYPE_RGB ||
        type == PNG_COLOR_TYPE_RGBA) << "png bad type: " << path;
  png_read_update_info(png_, info_);
  rows = (png_byte**)malloc(sizeof(png_byte*) * height);
  for (int y = 0; y < height; y++) {
    rows[y] = (png_byte*)malloc(png_get_rowbytes(png_, info_));
  }
  png_read_image(png_, rows);
  free(png_);
  free(info_);
  fclose(fp);
  std::vector<Pixel> pixels;
  for (int y = 0; y < height; ++y) {
    uint8_t* row = rows[y];
    if (type == PNG_COLOR_TYPE_RGBA) {
      for (int x = 0; x < width * 4; x += 4) {
        pixels.emplace_back(Color256(row[x]), Color256(row[x+1]),
                            Color256(row[x+2]), Color256(row[x+3]));
      }
    } else {
      for (int x = 0; x < width * 3; x += 3) {
        pixels.emplace_back(Color256(row[x]), Color256(row[x+1]),
                            Color256(row[x+2]));
      }
    }
    free(row);
  }
  free(rows);
  return Graphic(width, height, std::move(pixels));
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
