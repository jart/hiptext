#include "jpeg.h"

#include <csetjmp>

#include <glog/logging.h>
#include <jpeglib.h>

#include "pixel.h"
#include "graphic.h"

static inline float f8(uint8_t c) {
  return (float)c / 255.0;
}

static void OnError(j_common_ptr cinfo) {
  char buffer[JMSG_LENGTH_MAX];
  cinfo->err->format_message(cinfo, buffer);
  LOG(FATAL) << "bad jpeg: " << buffer;
}

Graphic LoadJPEG(const std::string& path) {
  FILE* fp = fopen(path.c_str(), "rb");
  PCHECK(fp) << path;
  jpeg_decompress_struct cinfo;
  jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jerr.error_exit = OnError;
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  CHECK(jpeg_read_header(&cinfo, TRUE) == JPEG_HEADER_OK);
  CHECK(jpeg_start_decompress(&cinfo) == TRUE);
  int stride = cinfo.output_width * cinfo.output_components;
  uint8_t* line = (uint8_t*)malloc(stride);
  uint8_t* buffer[] = { line };
  std::vector<Pixel> pixels;
  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    for (int n = 0; n < stride; n += cinfo.output_components) {
      pixels.push_back(rgb256(line[n], line[n + 1], line[n + 2]));
    }
  }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);
  return Graphic(cinfo.output_width, cinfo.output_height, std::move(pixels));
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
