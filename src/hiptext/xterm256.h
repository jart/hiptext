// hiptext - Image to Text Converter
// By Justine Tunney

#ifndef HIPTEXT_XTERM256_H_
#define HIPTEXT_XTERM256_H_

#include <cstdint>

class Pixel;

extern const Pixel g_xterm[256];
extern const uint8_t g_xterm_reverse[6][6][6];
uint8_t rgb_to_xterm(const Pixel& pix, int begin, int end);
uint8_t rgb_to_xterm16(const Pixel& pix);
uint8_t rgb_to_xterm256(const Pixel& pix);

#endif  // HIPTEXT_XTERM256_H_

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
