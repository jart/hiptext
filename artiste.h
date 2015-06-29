// hiptext - Image to Text Converter
// Serene Han

#ifndef HIPTEXT_ARTISTE_H_
#define HIPTEXT_ARTISTE_H_

#include <functional>
#include <ostream>

#include "unicode.h"

class Movie;
class Graphic;

using RenderAlgorithm = std::function<void(std::ostream&, const Graphic&)>;

class Artiste {  // The one who lives in your terminal.
 public:
  Artiste(std::ostream& output, std::istream& input,
          RenderAlgorithm algorithm, bool duopixel, bool use_sixel);
  ~Artiste();
  // The Artiste refuses such mimicry. (As expected of a hippy.)
  Artiste(const Artiste& a) = delete;
  void operator=(const Artiste& a) = delete;

  void PrintImage(Graphic graphic);
  void PrintMovie(Movie movie);

  void GenerateSpectrum();

  inline int term_width() const { return term_width_; }
  inline int term_height() const { return term_height_; }

  void ShowCursor();
  void HideCursor();
  void ResetCursor();

 private:
  void ComputeDimensions(double media_ratio);

  std::ostream& output_;
  RenderAlgorithm algorithm_;
  bool duo_pixel_;  // Some algorithms improve vertical resolution.

  int term_width_;
  int term_height_;
  double user_ratio_ = 0;
  double true_ratio_ = 0;
  int width_ = -1;  // Final output dimensions.
  int height_ = -1;

  bool cursor_saved_ = false;
};

#endif  // HIPTEXT_ARTISTE_H_

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
