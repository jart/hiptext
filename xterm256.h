#ifndef HIPTEXT_XTERM256_H_
#define HIPTEXT_XTERM256_H_

#include <cstdint>
#include "pixel.h"

extern Pixel g_xterm[256];
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
