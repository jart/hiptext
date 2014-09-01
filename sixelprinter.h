// hiptext - Image to Text Converter
// By Justine Tunney

#ifndef HIPTEXT_SIXELPRINTER_H_
#define HIPTEXT_SIXELPRINTER_H_

#include <ostream>

// A wrapper around cout that generates DECSIXEL escape codes.
class SixelPrinter {
 public:
  explicit SixelPrinter(std::ostream& out, int colors,
                        bool is8bit, bool bgprint, int bg);

  template<typename T>
  inline SixelPrinter& operator<<(const T& val) {
    out_ << val;
    return *this;
  }

  void PrintPixel(int n);
  void Start();
  void End();
  void LineFeed();

 private:
  void DefineColor(int n);

  std::ostream& out_;
  int colors_;
  bool is8bit_;   // whether the terminal accepts 8bit control (C1)
  bool bgprint_;
  int bg_;
  int cache_;
  int count_;
  int sixel_offset_;
  char slots_[256];
};

#endif  // HIPTEXT_SIXELPRINTER_H_

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
