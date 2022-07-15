#ifndef HIPTEXT_X11_H_
#define HIPTEXT_X11_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "hiptext/graphic.h"
#include "hiptext/input.h"

class X11 {
 public:
  explicit X11();
  X11(X11&& x11);
  X11(const X11& x11) = delete;
  void operator=(const X11& x11) = delete;

  Graphic Next();

  inline int width() const { return width_; }
  inline int height() const { return height_; }
  inline bool done() const { return done_; }

  // Make C++11 range-based loops work.
  struct iterator {
    Graphic operator*() { return x11_->Next(); }
    const iterator& operator++() { return *this; }
    bool operator!=(const iterator&) const { return !x11_->done(); }
    X11* x11_;
  };
  iterator begin() { return iterator{this}; }
  iterator end() { return iterator{this}; }

  void InitializeMain();
  void TermUpdate(int width, int height);
  void CreateImage();
  void DestroyImage();
  bool HandleInput();
  void Contain();
  void Zoom(int);
  void WaitABit(int);

  double zoom_factor = 3.0 / 2.0;

  int term_width;
  int term_height;
  int grab_x = 0;
  int grab_y = 0;
  int grab_width;
  int grab_height;
  // The number of X pixels representing a TTY 'pixel'
  int tty_pixel_x_size;
  int tty_pixel_y_size;

 private:
  bool IsPointerPixel(int x, int y, int pointer_x, int pointer_y);

  bool done_ = false;  // True when media is complete.

  int width_;  // Desired final size. Defaults to natural context.
  int height_;

  char *dpyname = ":0";
  Display *dpy;
  Screen  *scr;
  XImage *ximage;
  unsigned depth = 0;

  Input input;
  GC gc;
};

#endif  // HIPTEXT_X11_H_
