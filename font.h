// hiptext - Image to Text Converter
// Copyright (c) 2013 Justine Tunney

#ifndef HIPTEXT_FONT_H_
#define HIPTEXT_FONT_H_

class Graphic;
class Pixel;

void InitFont();
Graphic LoadLetter(wchar_t letter, const Pixel& fg, const Pixel& bg);

#endif  // HIPTEXT_FONT_H_
