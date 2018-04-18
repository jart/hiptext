#include "hiptext/x11.h"
#include "hiptext/pixel.h"
#include "hiptext/input.h"

X11::X11() {}

Graphic X11::Next() {
  unsigned int long pixel;
  std::vector<Pixel> pixels;
  int _, pointer_x, pointer_y;
  unsigned int u_;
  Window w;

  // Get the coords of the mouse on the desktop
  XQueryPointer(
    dpy,
    RootWindowOfScreen(scr),
    &w,
    &w,
    &pointer_x,
    &pointer_y,
    &_, &_, &u_
  );

  // Get a screengrab of the desktop, or a portion of the desktop
  XGetSubImage(
    dpy, RootWindowOfScreen(scr),
    grab_x, grab_y, grab_width, grab_height,
    AllPlanes, ZPixmap, ximage, 0, 0
  );

  for (int y = 0; y < ximage->height; y++) {
    for (int x = 0; x < ximage->width; x++) {
      pixel = XGetPixel(ximage, x, y);
      if (X11::IsPointerPixel(x, y, pointer_x, pointer_y)){
        pixels.emplace_back(1.0, 1.0, 1.0);
      } else {
        pixels.emplace_back(
          ((pixel >> 16) & 0x0000ff) / 255.0,
          ((pixel >> 8) & 0x0000ff)  / 255.0,
          ((pixel) & 0x0000ff)       / 255.0
        );
      }
    }
  }

  return Graphic(grab_width, grab_height, std::move(pixels));
}

/*
 * Overlay the mouse pointer to the size of a single TTY pixel when
 * mapping pixels to the terminal.
 */
bool X11::IsPointerPixel(int x, int y, int pointer_x, int pointer_y) {
  // Convert x and y from relative to absolute
  x = grab_x + x;
  y = grab_y + y;
  // Multiple tty pixel size by 5 for worse case scenarios when BilinearScale()
  // can't average the colour such that it displays in the terminal. Ideally this
  // should be implemented in the various render algorithms.
  bool within_x_region = ((x - pointer_x) * (x - pointer_x) < tty_pixel_x_size * 5);
  bool within_y_region = ((y - pointer_y) * (y - pointer_y) < tty_pixel_y_size * 5);
  return within_x_region && within_y_region;
}

bool X11::HandleInput() {
  int k;
  bool screen_grab_change = false;
  int pan_shift_x = tty_pixel_x_size * 3;
  int pan_shift_y = tty_pixel_y_size * 3;

  if (input.KbHit()) {
    k = input.GetCh();
    LOG(INFO) << k;
    switch (k){
      case 43: // +
        X11::Zoom(1);
        break;
      case 45: // -
        X11::Zoom(-1);
        break;
      case 65: // UP ARROW
        grab_y = grab_y - pan_shift_y;
        break;
      case 66: // DOWN ARROW
        grab_y = grab_y + pan_shift_y;
        break;
      case 68: // LEFT ARROW
        grab_x = grab_x - pan_shift_x;
        break;
      case 67: // RIGHT ARROW
        grab_x = grab_x + pan_shift_x;
        break;
    }
    screen_grab_change = true;
    X11::Contain();
    X11::CreateImage();
  }
  return screen_grab_change;
}

void X11::Zoom(int direction) {
  int centre_x = grab_x + (grab_width / 2);
  int centre_y = grab_y + (grab_height / 2);
  // Zoom in
  if (direction > 0) {
    grab_width = grab_width / zoom_factor;
    grab_height = grab_height / zoom_factor;
  }
  // Zoom out
  if (direction < 0) {
    grab_width = grab_width * zoom_factor;
    grab_height = grab_height * zoom_factor;
  }
  // Zoom anchored on the centre of the screen
  grab_x = centre_x - (grab_width / 2);
  grab_y = centre_y - (grab_height / 2);
}

void X11::Contain() {
  if (grab_width > scr->width)
    grab_width = scr->width;
  if (grab_height > scr->height)
    grab_height = scr->height;
  tty_pixel_x_size = grab_width / term_width;
  tty_pixel_y_size = grab_height / term_height;

  if (grab_x < 0)
    grab_x = 0;
  if (grab_x + grab_width > scr->width)
    grab_x = scr->width -grab_width;
  if (grab_y < 0 || (grab_y + grab_height > scr->height))
    grab_y = 0;
  if (grab_y + grab_height > scr->height)
    grab_y = scr->height - grab_height;
}

void X11::InitializeMain() {
  if (!(dpy = XOpenDisplay(dpyname))) {
    perror("Cannot open display");
    exit(-1);
  }

  input = Input();
  input.InitializeMain();

  scr       = DefaultScreenOfDisplay(dpy);
  depth     = DefaultDepthOfScreen(scr);
  grab_width  = scr->width;
  grab_height = scr->height;

  XGCValues gcv;
  gcv.plane_mask     = AllPlanes;
  gcv.subwindow_mode = IncludeInferiors;
  gcv.function       = GXcopy;
  gcv.foreground     = WhitePixelOfScreen(scr);
  gcv.background     = BlackPixelOfScreen(scr);
  gc                 = XCreateGC(dpy, RootWindowOfScreen(scr),
                                 GCFunction |
                                 GCPlaneMask |
                                 GCSubwindowMode |
                                 GCForeground |
                                 GCBackground,
                                 &gcv);
  X11::CreateImage();
}

void X11::TermUpdate(int width, int height) {
  term_width = width;
  term_height = height;
  X11::Contain();
}


// Prepare a variable for holding the screengrabs
void X11::CreateImage() {
  char *data = (char *)malloc(
    BitmapUnit(dpy) / 8 * grab_width * grab_height
  );

  ximage = XCreateImage(
    dpy,
    DefaultVisualOfScreen(scr),
    depth,
    ZPixmap, 0, data,
    grab_width, grab_height, 32, 0
  );

  if (ximage == NULL) {
    perror("XCreateImage");
    exit(-1);
  }
}

void X11::DestroyImage() {
  free(ximage->data);
  ximage->data = NULL;
  XDestroyImage(ximage);
  input.Destructor();
}

