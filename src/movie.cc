// hiptext - Image to Text Converter
// By Serene Han & Justine Tunney

#include "hiptext/movie.h"

#include <cstring>

#include <glog/logging.h>
extern "C" {  // ffmpeg hates C++ and won't put this in their headers.
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include "hiptext/graphic.h"
#include "hiptext/pixel.h"

#if (LIBAVFORMAT_VERSION_INT) > AV_VERSION_INT(55, 16, 0)
#define PIX_FMT_RGB24 AV_PIX_FMT_RGB24
#define avcodec_alloc_frame() av_frame_alloc()
#define av_free_packet(packet) av_packet_unref(packet)
#endif

Movie::Movie(const std::string& path) {
  format_ = avformat_alloc_context();

  // Fetch basic metadata.
  CHECK_EQ(0, avformat_open_input(&format_, path.data(), nullptr, nullptr));
  CHECK_GE(avformat_find_stream_info(format_, nullptr), 0);
  av_dump_format(format_, 0, path.data(), false);

  // Make sure it contains a video stream.
  video_stream_ = av_find_best_stream(
      format_, AVMEDIA_TYPE_VIDEO, -1, -1, &codec_, 0);
  CHECK_GE(video_stream_, 0) << "Couldn't find a video stream in: " << path;
  context_ = format_->streams[video_stream_]->codec;
  LOG(INFO) << "Native dimensions: " << context_->width << "x"
            << context_->height;
  width_ = context_->width;
  height_ = context_->height;
}

Movie::~Movie() {
  if (buffer_)    av_free(buffer_);
  if (frame_)     av_free(frame_);
  if (frame_rgb_) av_free(frame_rgb_);
  if (context_)   avcodec_close(context_);
  if (format_)    avformat_close_input(&format_);
}

void Movie::PrepareRGB(int width, int height) {
  // Extract codec and decoding context for RGB output.
  // Should be called once the correct RGB output dimensions are known,
  // and prior to fetching video frames with Next().
  CHECK(width > 0 && height > 0)
      << "Invalid dimensions: " << width << "x" <<  height;
  width_ = width;
  height_ = height;

  // Prepare context for scaling and converting to RGB.
  sws_ = sws_getContext(context_->width, context_->height, context_->pix_fmt,
                        width_, height_, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR,
                        nullptr, nullptr, nullptr);
  CHECK(codec_ = avcodec_find_decoder(context_->codec_id))
      << "Unsupported codec.\n";
  CHECK_GE(avcodec_open2(context_, codec_, nullptr), 0)
      << "Could not open codec.\n";

  // Allocate Raw + RGB frame buffers.
  CHECK(frame_ = av_frame_alloc());
  CHECK(frame_rgb_ = av_frame_alloc());
  int rgb_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width_, height_);
  buffer_ = static_cast<uint8_t*>(av_malloc(rgb_bytes));
  LOG(INFO) << "RGB Buffer: " << rgb_bytes << " bytes.";
  int prep = avpicture_fill(reinterpret_cast<AVPicture*>(frame_rgb_),
                            buffer_, AV_PIX_FMT_RGB24, width_, height_);
  CHECK_GE(prep, 0) << "Failed to prepare RGB buffer.";
  LOG(INFO) << "RGB dimensions: " << width_  << "x" << height_;
}

Movie::Movie(Movie&& movie) {
  memcpy(reinterpret_cast<void*>(this),
         reinterpret_cast<void*>(&movie),
         sizeof(movie));
  memset(reinterpret_cast<void*>(&movie), 0, sizeof(movie));
}

Graphic Movie::Next() {
  int found = 0;   // Find immediate next video frame.
  while (!found) {
    AVPacket packet;
    if (av_read_frame(format_, &packet) < 0) {
      done_ = true;
      LOG(INFO) << "Movie complete.";
      return Graphic(width_, height_);
    }
    if (packet.stream_index == video_stream_) {
      avcodec_decode_video2(context_, frame_, &found, &packet);
    }
    av_packet_unref(&packet);
  }

  // Convert Raw to RGB.
  sws_scale(sws_, frame_->data,
            frame_->linesize, 0, context_->height,
            frame_rgb_->data, frame_rgb_->linesize);
  int data_width = frame_rgb_->linesize[0];
  CHECK(data_width == 3 * width_)
      << "RGB Frame buffer misallocated. Stride was " << data_width
      << " bytes, while RGB pixel width required " << 3*width_ << " bytes.";

  // Convert RGB to Hiptext representation.
  std::vector<Pixel> pixels;
  for (int y = 0; y < height_; ++y) {
    uint8_t* row = frame_rgb_->data[0] + data_width * y;
    for (int x = 0; x < data_width; x += 3) {
      pixels.emplace_back(row[x], row[x + 1], row[x + 2]);
    }
  }
  CHECK(static_cast<int>(pixels.size()) == width_ * height_);
  return Graphic(width_, height_, std::move(pixels));
}

void Movie::InitializeMain() {
  avcodec_register_all();
  av_register_all();
}
