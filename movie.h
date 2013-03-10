// hiptext - Image to Text Converter
// By Serene Han & Justine Tunney

#ifndef HIPTEXT_MOVIE_H_
#define HIPTEXT_MOVIE_H_

#include <string>

#include "graphic.h"

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;
struct AVFrame;
struct SwsContext;

class Movie {
 public:
  explicit Movie(const std::string& path) : Movie(path, 0) {}
  ~Movie();
  Movie(const std::string& path, int width);
  Movie(Movie&& movie);
  Movie(const Movie& movie) = delete;
  void operator=(const Movie& movie) = delete;

  Graphic Next();
  inline bool done() const { return done_; }

  // Make C++11 range-based loops work.
  struct iterator {
    Graphic operator*() { return movie_->Next(); }
    const iterator& operator++() { return *this; }
    bool operator!=(const iterator&) const { return !movie_->done(); }
    Movie* movie_;
  };
  iterator begin() { return iterator{this}; }
  iterator end() { return iterator{this}; }

  static void InitializeMain();

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

  int width_;  // Desired final size. Defaults to natural context.
  int height_;
};

#endif  // HIPTEXT_MOVIE_H_
