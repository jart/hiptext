#include "png.h"

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

#include <glog/logging.h>

#include "pixel.h"
#include "graphic.h"

Graphic LoadPNG(const std::string& path) {
  png_struct* png;
  png_info* info;
  uint8_t** rows;
  uint8_t header[8];
  FILE* fp = fopen(path.c_str(), "rb");
  PCHECK(fp) << path;
  PCHECK(fread(header, 8, 1, fp) == 1) << path;
  CHECK(png_sig_cmp(header, 0, 8) == 0) << "bad png file: " << path;
  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(png);
  CHECK(setjmp(png_jmpbuf(png)) == 0) << "png read error: " << path;
  png_init_io(png, fp);
  png_set_sig_bytes(png, 8);
  png_read_info(png, info);
  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  int type = png_get_color_type(png, info);
  CHECK(type == PNG_COLOR_TYPE_RGB ||
        type == PNG_COLOR_TYPE_RGBA) << "png bad type: " << path;
  png_read_update_info(png, info);
  rows = (png_byte**)malloc(sizeof(png_byte*) * height);
  for (int y = 0; y < height; y++) {
    rows[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
  }
  png_read_image(png, rows);
  free(png);
  free(info);
  PCHECK(fclose(fp) == 0) << path;
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

void WritePNG(const Graphic& graphic, const std::string& path) {
  png_struct* png;
  png_info* info;
  FILE* fp = fopen(path.c_str(), "wb");
  PCHECK(fp) << path;
  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(png);
  CHECK(setjmp(png_jmpbuf(png)) == 0) << "png write error: " << path;
  png_init_io(png, fp);
  png_set_IHDR(png, info, graphic.width(), graphic.height(),
               8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_text title;
  title.compression = PNG_TEXT_COMPRESSION_NONE;
  title.key = (char*)"Title";
  title.text = (char*)"Justine says hello :)";
  png_set_text(png, info, &title, 1);
  png_write_info(png, info);
  uint8_t* row = (uint8_t*)malloc(png_get_rowbytes(png, info));
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      const Pixel& pixel = graphic.Get(x, y);
      row[x * 4 + 0] = ColorTo256(pixel.red());
      row[x * 4 + 1] = ColorTo256(pixel.green());
      row[x * 4 + 2] = ColorTo256(pixel.blue());
      row[x * 4 + 3] = ColorTo256(pixel.alpha());
    }
    png_write_row(png, row);
  }
  free(row);
  png_free_data(png, info, PNG_FREE_ALL, -1);
  png_destroy_write_struct(&png, nullptr);
  PCHECK(fclose(fp) == 0) << path;
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
