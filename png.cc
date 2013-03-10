// hiptext - Image to Text Converter
// By Justine Tunney

#include "png.h"

#include <memory>
#include <vector>

#include <glog/logging.h>
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

#include "graphic.h"
#include "pixel.h"

Graphic LoadPNG(const std::string& path) {
  FILE* fp = fopen(path.data(), "rb");
  PCHECK(fp) << path;
  uint8_t header[8];
  PCHECK(fread(header, 8, 1, fp) == 1) << path;
  CHECK_EQ(0, png_sig_cmp(header, 0, 8)) << "bad png file: " << path;
  png_struct* png = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  CHECK_NOTNULL(png);
  png_info* info = png_create_info_struct(png);
  CHECK_NOTNULL(info);
  CHECK_EQ(0, setjmp(png_jmpbuf(png))) << "png read error: " << path;
  png_init_io(png, fp);
  png_set_sig_bytes(png, 8);
  png_read_info(png, info);
  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  int type = png_get_color_type(png, info);
  CHECK(type == PNG_COLOR_TYPE_RGB ||
        type == PNG_COLOR_TYPE_RGBA) << "png bad type: " << path;
  png_read_update_info(png, info);
  std::unique_ptr<uint8_t*[]> rows(new uint8_t*[height]);
  for (int y = 0; y < height; ++y) {
    rows[y] = new uint8_t[png_get_rowbytes(png, info)];
  }
  png_read_image(png, rows.get());
  free(png);
  free(info);
  PCHECK(fclose(fp) == 0) << path;
  std::vector<Pixel> pixels;
  for (int y = 0; y < height; ++y) {
    uint8_t* row = rows[y];
    if (type == PNG_COLOR_TYPE_RGBA) {
      for (int x = 0; x < width * 4; x += 4) {
        pixels.emplace_back(row[x], row[x+1], row[x+2], row[x+3]);
      }
    } else {
      for (int x = 0; x < width * 3; x += 3) {
        pixels.emplace_back(row[x], row[x+1], row[x+2]);
      }
    }
    delete[] row;
  }
  return Graphic(width, height, std::move(pixels));
}

void WritePNG(const Graphic& graphic, const std::string& path) {
  FILE* fp = fopen(path.c_str(), "wb");
  PCHECK(fp) << path;
  png_struct* png = png_create_write_struct(
      PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  CHECK_NOTNULL(png);
  png_info* info = png_create_info_struct(png);
  CHECK_NOTNULL(info);
  CHECK_EQ(0, setjmp(png_jmpbuf(png))) << "png write error: " << path;
  png_init_io(png, fp);
  png_set_IHDR(png, info, graphic.width(), graphic.height(),
               8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_text title;
  title.compression = PNG_TEXT_COMPRESSION_NONE;
  title.key = const_cast<char*>("Title");
  title.text = const_cast<char*>("Justine Tunney says hello :)");
  png_set_text(png, info, &title, 1);
  png_write_info(png, info);
  std::unique_ptr<uint8_t[]> row(new uint8_t[png_get_rowbytes(png, info)]);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      const Pixel& pixel = graphic.Get(x, y);
      row[x * 4 + 0] = static_cast<uint8_t>(pixel.red() * 255);
      row[x * 4 + 1] = static_cast<uint8_t>(pixel.green() * 255);
      row[x * 4 + 2] = static_cast<uint8_t>(pixel.blue() * 255);
      row[x * 4 + 3] = static_cast<uint8_t>(pixel.alpha() * 255);
    }
    png_write_row(png, row.get());
  }
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
