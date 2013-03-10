// hiptext - Image to Text Converter
// Copyright (c) 2013 Serene Han & Justine Tunney

#ifndef HIPTEXT_MOVIE_H_
#define HIPTEXT_MOVIE_H_

#include <string>
#include <iterator>

#include "graphic.h"

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;
struct AVFrame;
struct SwsContext;

class Movie {
 public:
  Movie(const std::string& path);
  Movie(const std::string& path, int width);
  void Init(const std::string& path, int width);
  Graphic Next();

  struct iterator {
    Graphic operator*() { return movie_->Next(); }
    const iterator& operator++() { return *this; } 
    bool operator!=(const iterator&) const { return !movie_->Done(); }
    Movie* movie_;
  };
  iterator begin() { return iterator{this}; }
  iterator end() { return iterator{this}; }

  inline bool Done() { return done_; }

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
