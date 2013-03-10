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
  if (width) {  // Scale by aspect ratio if necessary.
    width_ = width;   
    height_ = width_ * context_->height / context_->width;
  } else {  // Otherwise default to native context.
    width_ = context_->width;
    height_ = context_->height;
  }
  CHECK(width_ > 0 && height_ > 0);

  sws_ = sws_getContext(context_->width, context_->height,
                        context_->pix_fmt, 
                        context_->width, context_->height,
                        PIX_FMT_RGB24, SWS_BICUBIC,
                        nullptr, nullptr, nullptr);
  codec_ = avcodec_find_decoder(context_->codec_id);
  CHECK(codec_) << "Unsupported codec.\n";
  CHECK(avcodec_open2(context_, codec_, nullptr) >= 0) << "Could not open codec.\n";


  LOG(INFO) << "CONTEXT width=" << context_->width
            << " height=" << context_->height;

  frame_ = avcodec_alloc_frame(); 
  CHECK(frame_ != nullptr);
  frame_rgb_ = avcodec_alloc_frame(); 
  CHECK(frame_rgb_ != nullptr);
  int rgb_bytes = avpicture_get_size(PIX_FMT_RGB24,
                                context_->width, context_->height);
  buffer_ = (uint8_t *)av_malloc(sizeof(uint8_t) * rgb_bytes);
  LOG(INFO) << "RGB buffer: " << rgb_bytes << " bytes.";
  int prep = avpicture_fill((AVPicture *)frame_rgb_, buffer_, PIX_FMT_RGB24,
                            context_->width, context_->height);
  CHECK(prep >= 0) << "Failed to prepare RGB buffer.";

  LOG(INFO) << "RGB width=" << frame_rgb_->width
            << " height=" << frame_rgb_->height;
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
  LOG(INFO) << "av_read_frame found a video frame. Finished=" << finished;

  // Convert av native format to RGB.
  sws_scale(sws_, frame_->data,
            frame_->linesize, 0, context_->height,
            frame_rgb_->data, frame_rgb_->linesize);
  // Convert RGB to hiptext representation.
  std::vector<Pixel> pixels;
  int data_width = frame_rgb_->linesize[0];
  for (int y = 0; y < context_->height; ++y) {
    uint8_t* row = frame_rgb_->data[0] + y * data_width;
    for (int x = 0; x < context_->width; x += 1) {
      pixels.emplace_back(row[x], row[x+1], row[x+2]);
    }
  }
  return Graphic(context_->width, context_->height, std::move(pixels));
}
