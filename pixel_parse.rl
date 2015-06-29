// hiptext - Image to Text Converter
// By Justine Tunney

// Complete CSS3/X11 Color Parser Implementation (and more!)

#include "pixel.h"
#include "unused.h"
#include <vector>
#include <glog/logging.h>

%% machine css_color;
%% include css_color "css_color.rl";
%% write data;

// Assumes valid input and case insensitive.
int UnHex(char ch) {
  if (ch <= '9') {
    return ch - '0';
  } else if (ch <= 'F') {
    return ch - 'A' + 10;
  } else {
    return ch - 'a' + 10;
  }
}

// Expands 0..100 -> 0..255.
inline int Percent(int val) {
  return static_cast<int>(static_cast<float>(val) / 100.0 * 255.0);
}

Pixel::Pixel(const std::string& name) {
  MONUnusedParameter(css_color_first_final);
  MONUnusedParameter(css_color_error);
  MONUnusedParameter(css_color_en_main);
  CHECK(name.size() > 0);
  int cs;
  const char* f = name.data();
  const char* p = f;
  const char* pe = p + name.size();
  const char* eof = pe;
  const char* mark = p;
  int val;
  double fval = 1.0;
  std::vector<int> vals;
  %% write init;
  %% write exec;
  LOG(FATAL) << "Invalid color: " << name;
}

// For Emacs:
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:2
// c-basic-offset:2
// c-file-style:nil
// End:
// For VIM:
// vim:set expandtab softtabstop=2 shiftwidth=2 tabstop=2:
