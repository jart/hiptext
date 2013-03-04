// hiptext - Image to Text Converter
// Copyright (c) 2013 Justine Tunney

#ifndef HIPTEXT_XTERM256_H_
#define HIPTEXT_XTERM256_H_

#include <cstdint>

class Pixel;

extern Pixel g_xterm[256];
const Pixel& xterm_to_rgb(int code);
uint8_t rgb_to_xterm(const Pixel& pix, int begin, int end);
uint8_t rgb_to_xterm16(const Pixel& pix);
uint8_t rgb_to_xterm256(const Pixel& pix);
void PrintXterm256();

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
