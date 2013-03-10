// hiptext - Image to Text Converter
// Copyright (c) 2013 Serene Han & Justine Tunney

#ifndef HIPTEXT_MOVIE_H_
#define HIPTEXT_MOVIE_H_

#include <string>

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;
struct AVFrame;
struct SwsContext;

class Graphic;

class Movie {
 public:
  Movie(const std::string& path);
  Movie(const std::string& path, int width);
  void Init(const std::string& path, int width);
  Graphic Next();

  // Graphic get const;

 private:
  bool done_ = false;
  int video_stream_;
  uint8_t* buffer_;
  AVCodec* codec_;
  AVCodecContext* context_;
  AVFormatContext* format_;
  AVFrame* frame_; 
  AVFrame* frame_rgb_; 
  SwsContext* sws_;
  int width_;   // Desired final size. Defaults to natural context.
  int height_;
};

#endif  // HIPTEXT_MOVIE_H_
