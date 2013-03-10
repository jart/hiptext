// hiptext - Image to Text Converter
// Copyright (c) 2013 Serene Han & Justine Tunney

#include "movie.h"

#include <glog/logging.h>
extern "C" {  // ffmpeg hates C++ and won't put this in their headers.
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include "graphic.h"
#include "pixel.h"

Movie::Movie(const std::string& path) {
  Init(path, 0);
}

Movie::Movie(const std::string& path, int width) {
  Init(path, width);
}

void Movie::Init(const std::string& path, int width) {
  avcodec_register_all();
  av_register_all();
  format_ = avformat_alloc_context();

  // Basic information.
  CHECK(avformat_open_input(&format_, path.data(), nullptr, nullptr) == 0);
  CHECK(avformat_find_stream_info(format_, nullptr) >= 0);
  av_dump_format(format_, 0, path.data(), false);

  // Validate and find first video stream.
  video_stream_ = -1;
  for (unsigned i = 0; i < format_->nb_streams; ++i) {
    if (format_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_ = i;
      break;
    }
  }
  CHECK(video_stream_ >= 0);

  // Extract codec and decoding context for RGB output.
  context_ = format_->streams[video_stream_]->codec;
  LOG(INFO) << "Native dimensions: " << context_->width << "x" << context_->height;
  width_ = context_->width;
  height_ = context_->height;
  if (width) {  // Scale by aspect ratio if necessary.
    float scale = (float)width/(float)width_;
    LOG(INFO) << "Scale factor: " << scale;
    height_ *= scale;
    // height_ = context_->height / 2;
    width_ = width;
  }
  CHECK(width_ > 0 && height_ > 0) <<
      "Invalid dimensions: " << width_ << "x" <<  height_;

  sws_ = sws_getContext(context_->width, context_->height, context_->pix_fmt,
                        width_, height_,  PIX_FMT_RGB24,
                        SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
  codec_ = avcodec_find_decoder(context_->codec_id);
  CHECK(codec_) << "Unsupported codec.\n";
  CHECK(avcodec_open2(context_, codec_, nullptr) >= 0) << "Could not open codec.\n";

  frame_ = avcodec_alloc_frame(); 
  CHECK(frame_ != nullptr);
  frame_rgb_ = avcodec_alloc_frame(); 
  CHECK(frame_rgb_ != nullptr);

  // Prepare RGB buffer.
  int rgb_bytes = avpicture_get_size(PIX_FMT_RGB24, width_, height_);
  buffer_ = (uint8_t *)av_malloc(rgb_bytes);
  LOG(INFO) << "RGB Buffer: " << rgb_bytes << " bytes.";
  int prep = avpicture_fill((AVPicture *)frame_rgb_,
                            buffer_, PIX_FMT_RGB24,
                            width_, height_);
  CHECK(prep >= 0) << "Failed to prepare RGB buffer.";
  LOG(INFO) << "RGB dimensions: " << width  << "x" << height_;
  done_ = false;
}

Graphic Movie::Next() {
  int finished = 0;   // Find immediate next video frame.
  while (!finished) {
    AVPacket packet;
    if (av_read_frame(format_, &packet) < 0) {
      done_ = true;
      LOG(INFO) << "Done reading.";
      return Graphic(0,0);
    }
    if (packet.stream_index == video_stream_) {
      avcodec_decode_video2(context_, frame_, &finished, &packet);
    }
    av_free_packet(&packet);
  }

  // Convert av native format to RGB.
  sws_scale(sws_, frame_->data,
            frame_->linesize, 0, context_->height,
            frame_rgb_->data, frame_rgb_->linesize);
  // Convert RGB to hiptext representation.
  LOG(INFO) << "av_read_frame: video frame. " << width_ << "x" << height_;
  std::vector<Pixel> pixels;
  int data_width = frame_rgb_->linesize[0];
  CHECK(data_width == 3*width_);
  for (int y = 0; y < height_; ++y) {
    LOG(INFO) << "Y is " << y;
    uint8_t* row = frame_rgb_->data[0] + data_width * y;
    int meow = 0;
    for (int x = 0; x < data_width; x += 3) {
      int r = row[x];
      int g = row[x+1];
      int b = row[x+2];
      CHECK(r < 256);
      CHECK(g < 256);
      CHECK(b < 256);
      if (r > 0) {
//        LOG(INFO) << "Pixel[" << (x/3) << ", " << y << "] = (" <<
///            r << ", " << g << ", " << b << ")"; 
        meow = 1;
      }
      pixels.emplace_back(r, g, b);
    }
    if (meow) LOG(INFO) << "lulz";
  }
  CHECK((int)pixels.size() == width_ * height_);
  return Graphic(width_, height_, std::move(pixels));
}
