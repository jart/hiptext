// hiptext - Image to Text Converter
// Copyright (c) 2013 Justine Tunney

#ifndef HIPTEXT_PNG_H_
#define HIPTEXT_PNG_H_

#include <string>

class Graphic;

Graphic LoadPNG(const std::string& path);
void WritePNG(const Graphic& graphic, const std::string& path);

#endif  // HIPTEXT_PNG_H_

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
