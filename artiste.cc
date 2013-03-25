// hiptext - Image to Text Converter
// Serene Han

#include "artiste.h"

#include <iostream>
#include <sys/ioctl.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "movie.h"

using std::cout;
using std::string;

DEFINE_int32(width, 0, "Width of rendering. Defaults to 0, in which case it "
           "automatically detects the terminal width. If height is not "
           "provided, it still maintains the aspect ratio. Cannot exceed the "
           "terminal width");
DEFINE_int32(height, 0, "Height of rendering. Defaults to 0, in which case it "
           "automatically maintains the aspect ratio with respect to width");
DEFINE_bool(equalize, false, "Use the histogram equalizer filter. You should "
            "use this when your image looks 'washed out' or grey when rendered "
            "in hiptext");
DEFINE_bool(stepthrough, false, "Whether to wait for human to press Return "
            "between frames. Only applicable to movie playbacks");

inline double RatioOf(int width, int height) {
  return static_cast<double>(width) / static_cast<double>(height);
}

Artiste::Artiste(std::ostream& output,
                 RenderAlgorithm algorithm,
                 bool duo_pixel)
    : output_(output), algorithm_(algorithm), duo_pixel_(duo_pixel) {
  winsize ws;
  PCHECK(ioctl(0, TIOCGWINSZ, &ws) == 0);
  // Users' concept of a "pixel" shall be as square as possible.
  // Therefore, double ws_row since characters approximate ~2:1rectangles.
  term_height_ = ws.ws_row * 2;
  term_width_ = ws.ws_col;

  // If user provides *both* FLAGS_width and FLAGS_height, remember their
  // desired ratio.
  if (FLAGS_width && FLAGS_height) {
    user_ratio_ = RatioOf(FLAGS_width, FLAGS_height);
    LOG(INFO) << "User enforced ratio: " << user_ratio_;
  }
}

Artiste::~Artiste() {
  CleanUp();
}

void Artiste::ComputeDimensions(double media_ratio) {
  CHECK(algorithm_) << "No algorithm selected.";
  // Compute optimal output RGB dimensions given:
  //
  // - Native resolution/aspect-ratio of input media,
  // - Terminal size limitations,
  // - User's preferred FLAGS_width and/or FLAGS_height.
  //
  // Should only be called once: after extracting the input media's aspect
  // ratio but prior to any rendering.
  double width = std::min(FLAGS_width ? FLAGS_width : term_width_,
                          term_width_);
  double height = std::min(FLAGS_height ? FLAGS_height : term_height_,
                           term_height_);
  true_ratio_ = user_ratio_ ? user_ratio_ : media_ratio;
  LOG(INFO) << "Aspect Ratio: " << true_ratio_;

  // Apply ratio both ways to ensure a fit.
  height = std::min(height, width / true_ratio_);
  width = std::min(width, height * true_ratio_);
  width_ = static_cast<int>(width);
  height_ = static_cast<int>(height) / (duo_pixel_ ? 1 : 2);

  LOG(INFO) << "Terminal Resolution: " << term_width_ << "x" << term_height_;
  LOG(INFO) << "Final Resolution (pixel-agnostic): " << width_ << "x"
            << height_;
}

void Artiste::PrintImage(Graphic graphic) {
  // Image decoders biject 1:1 to Hiptext's raw RGB representation,
  // so must be scaled once more prior to rendering.
  ComputeDimensions(RatioOf(graphic.width(), graphic.height()));
  if (FLAGS_equalize) {
    // graphic.ToYUV();
    graphic.Equalize();
    // graphic.FromYUV();
  }
  algorithm_(output_, graphic.BilinearScale(width_, height_));
}

void Artiste::PrintMovie(Movie movie) {
  // Movie files sws_scale to size in real-time, so the final
  // dimensions should be precomputed to avoid redundant scaling.
  ComputeDimensions(RatioOf(movie.width(), movie.height()));
  movie.PrepareRGB(width_, height_);
  HideCursor();
  for (auto graphic : movie) {
    ResetCursor();
    if (FLAGS_equalize) {
      // graphic.ToYUV();
      graphic.Equalize();
      // graphic.FromYUV();
    }
    algorithm_(output_, std::move(graphic));
    if (FLAGS_stepthrough) {
      string lulz;
      std::getline(std::cin, lulz);
    }
  }
  ShowCursor();
}

void Artiste::GenerateSpectrum() {
  int width = term_width_;
  int height = term_height_ * 2 - 2;

  int bar_width = static_cast<double>(width) * 0.05;
  int spec_width = width - bar_width * 4;
  double hh = static_cast<double>(height) / 2.0;
  Graphic res(width, height);
  for (int y = 0; y < height; ++y) {
    double fy = static_cast<double>(y);

    // Render the large color spectrum.
    for (int x = 0; x < spec_width; ++x) {
      double fx = static_cast<double>(x);
      res.Get(x, y) = Pixel(
          fx / spec_width,
          (y > hh) ? 1.0 : fy / (hh),
          (y < hh) ? 1.0 : 1.0 - (fy - (hh)) / (hh)).FromHSV();
    }

    // Render the grey bar.
    int offset = spec_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) =
          Pixel(0.0, 0.0, fy / height).FromHSV();
    }

    // Render the red/white gradient bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(1.0, fy / height, fy / height);
    }

    // Render the green/white gradient bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(fy / height, 1.0, fy / height);
    }

    // Render the blue/white gradient bar.
    offset += bar_width;
    for (int x = 0; x < bar_width; ++x) {
      res.Get(x + offset, y) = Pixel(fy / height, fy / height, 1.0);
    }
  }
  PrintImage(res);
}

void Artiste::HideCursor() {
  cursor_saved_ = true;
  output_ << "\x1b[s";     // ANSI save cursor position.
  output_ << "\x1b[?25l";  // ANSI make cursor invisible.
}

void Artiste::ShowCursor() {
  output_ << "\x1b[u";     // ANSI restore cursor position.
  output_ << "\x1b[?25h";  // ANSI make cursor visible.
  cursor_saved_ = false;
}

void Artiste::ResetCursor() {
  output_ << "\x1b[H";     // ANSI put cursor in top left.
}

void Artiste::CleanUp() {
  // Reset terminal state in all cases.
  if (cursor_saved_) {
    ShowCursor();
  }
}
