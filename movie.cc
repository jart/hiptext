// hiptext - Image to Text Converter
// Copyright (c) 2013 Justine Tunney & Serene Han

#include "movie.h"

#include <glog/logging.h>
extern "C" {  // Because libav hates C++ and won't put this in their header.
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "graphic.h"
#include "pixel.h"


Movie::Movie(const std::string& path) : path_(path) {
}

bool Movie::Initialize() {
  avcodec_register_all();
  av_register_all();

  CHECK(avformat_open_input(&format_, path_.data(), nullptr, nullptr) == 0);
  CHECK(avformat_find_stream_info(format_, nullptr) >= 0);
  av_dump_format(format_, 0, path_.data(), false);

  return true;
}

Graphic Movie::Next() {
  return Graphic(0, 0);
}
