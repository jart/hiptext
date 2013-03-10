// hiptext - Image to Text Converter
// Copyright (c) 2013 Serene Han & Justine Tunney

#ifndef HIPTEXT_MOVIE_H_
#define HIPTEXT_MOVIE_H_

#include <string>

class Graphic;
struct AVCodec;
struct AVFormatContext;
struct AVCodecContext;

class Movie {
 public:
  Movie(const std::string& path);
  bool Initialize();
  Graphic Next();

 private:
  std::string path_;
  AVFormatContext* format_;
  // AVCodec* codec_;
  // AVCodecContext* context_;
};

#endif  // HIPTEXT_MOVIE_H_
