// hiptext - Image to Text Converter
// By Justine Tunney

// Complete CSS3/X11 Color Parser Implementation (and more!)

#include "pixel.h"
#include <vector>
#include <glog/logging.h>

%%{
  machine pixel_color;
  write data;
}%%

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
  %%{
    machine pixel_color;

    action hex_rgb {
      *this = { UnHex(p[-6]) * 16 + UnHex(p[-5]),
                UnHex(p[-4]) * 16 + UnHex(p[-3]),
                UnHex(p[-2]) * 16 + UnHex(p[-1]) };
      return;
    }

    action hex_rgba {
      *this = { UnHex(p[-8]) * 16 + UnHex(p[-7]),
                UnHex(p[-6]) * 16 + UnHex(p[-5]),
                UnHex(p[-4]) * 16 + UnHex(p[-3]),
                UnHex(p[-2]) * 16 + UnHex(p[-1]) };
      return;
    }

    action hex_rgb_short {
      *this = { UnHex(p[-3]) * 16 + UnHex(p[-3]),
                UnHex(p[-2]) * 16 + UnHex(p[-2]),
                UnHex(p[-1]) * 16 + UnHex(p[-1]) };
      return;
    }

    action rgb {
      DCHECK_EQ(3, vals.size());
      *this = {vals[0], vals[1], vals[2]};
      alpha_ = fval;
      return;
    }

    action hsl {
      DCHECK_EQ(3, vals.size());
      *this = {vals[0], vals[1], vals[2]};
      alpha_ = fval;
      FromHSL();
      return;
    }

    action hsv {
      DCHECK_EQ(3, vals.size());
      *this = {vals[0], vals[1], vals[2]};
      alpha_ = fval;
      FromHSV();
      return;
    }

    sign = '-' | '+' ;
    real = sign? [.0-9]* ( 'e'i sign? digit+ )? ;
    atof = real   >{ mark = p; } %{ fval = std::stod(mark);      } ;
    atoi = digit+ >{ val = 0;  } ${ val = val * 10 + (*p - '0'); } ;

    number = '-' atoi  %{ vals.push_back(0);   }
           | atoi      %{ vals.push_back(val); }
           | atoi '%'  %{ CHECK(val <= 100); vals.push_back(Percent(val)); }
           ;

    opp = '('  space*       ;
    clp =      space*   ')' ;
    com = space* ',' space* ;

    main := '#' xdigit{3} %hex_rgb_short
         |  '#' xdigit{6} %hex_rgb
         |  '#' xdigit{8} %hex_rgba
         |  'rgb'i  opp number com number com number          clp %rgb
         |  'rgba'i opp number com number com number com atof clp %rgb
         |  'hsl'i  opp number com number com number          clp %hsl
         |  'hsla'i opp number com number com number com atof clp %hsl
         |  'hsv'i  opp number com number com number          clp %hsv
         |  'hsva'i opp number com number com number com atof clp %hsv
         |  'transparent'i          %{ *this = {};              return; }
         |  'black'i                %{ *this = {  0,   0,   0}; return; }
         |  'white'i                %{ *this = {255, 255, 255}; return; }
         |  'aliceblue'i            %{ *this = {240, 248, 255}; return; }
         |  'antiquewhite1'i        %{ *this = {255, 239, 219}; return; }
         |  'antiquewhite2'i        %{ *this = {238, 223, 204}; return; }
         |  'antiquewhite3'i        %{ *this = {205, 192, 176}; return; }
         |  'antiquewhite4'i        %{ *this = {139, 131, 120}; return; }
         |  'antiquewhite'i         %{ *this = {250, 235, 215}; return; }
         |  'aquamarine1'i          %{ *this = {127, 255, 212}; return; }
         |  'aquamarine2'i          %{ *this = {118, 238, 198}; return; }
         |  'aquamarine3'i          %{ *this = {102, 205, 170}; return; }
         |  'aquamarine4'i          %{ *this = { 69, 139, 116}; return; }
         |  'aquamarine'i           %{ *this = {127, 255, 212}; return; }
         |  'azure1'i               %{ *this = {240, 255, 255}; return; }
         |  'azure2'i               %{ *this = {224, 238, 238}; return; }
         |  'azure3'i               %{ *this = {193, 205, 205}; return; }
         |  'azure4'i               %{ *this = {131, 139, 139}; return; }
         |  'azure'i                %{ *this = {240, 255, 255}; return; }
         |  'beige'i                %{ *this = {245, 245, 220}; return; }
         |  'bisque1'i              %{ *this = {255, 228, 196}; return; }
         |  'bisque2'i              %{ *this = {238, 213, 183}; return; }
         |  'bisque3'i              %{ *this = {205, 183, 158}; return; }
         |  'bisque4'i              %{ *this = {139, 125, 107}; return; }
         |  'bisque'i               %{ *this = {255, 228, 196}; return; }
         |  'blanchedalmond'i       %{ *this = {255, 235, 205}; return; }
         |  'blue1'i                %{ *this = {  0,   0, 255}; return; }
         |  'blue2'i                %{ *this = {  0,   0, 238}; return; }
         |  'blue3'i                %{ *this = {  0,   0, 205}; return; }
         |  'blue4'i                %{ *this = {  0,   0, 139}; return; }
         |  'blue'i                 %{ *this = {  0,   0, 255}; return; }
         |  'blueviolet'i           %{ *this = {138,  43, 226}; return; }
         |  'brown1'i               %{ *this = {255,  64,  64}; return; }
         |  'brown2'i               %{ *this = {238,  59,  59}; return; }
         |  'brown3'i               %{ *this = {205,  51,  51}; return; }
         |  'brown4'i               %{ *this = {139,  35,  35}; return; }
         |  'brown'i                %{ *this = {165,  42,  42}; return; }
         |  'burlywood1'i           %{ *this = {255, 211, 155}; return; }
         |  'burlywood2'i           %{ *this = {238, 197, 145}; return; }
         |  'burlywood3'i           %{ *this = {205, 170, 125}; return; }
         |  'burlywood4'i           %{ *this = {139, 115,  85}; return; }
         |  'burlywood'i            %{ *this = {222, 184, 135}; return; }
         |  'cadetblue1'i           %{ *this = {152, 245, 255}; return; }
         |  'cadetblue2'i           %{ *this = {142, 229, 238}; return; }
         |  'cadetblue3'i           %{ *this = {122, 197, 205}; return; }
         |  'cadetblue4'i           %{ *this = { 83, 134, 139}; return; }
         |  'cadetblue'i            %{ *this = { 95, 158, 160}; return; }
         |  'chartreuse1'i          %{ *this = {127, 255,   0}; return; }
         |  'chartreuse2'i          %{ *this = {118, 238,   0}; return; }
         |  'chartreuse3'i          %{ *this = {102, 205,   0}; return; }
         |  'chartreuse4'i          %{ *this = { 69, 139,   0}; return; }
         |  'chartreuse'i           %{ *this = {127, 255,   0}; return; }
         |  'chocolate1'i           %{ *this = {255, 127,  36}; return; }
         |  'chocolate2'i           %{ *this = {238, 118,  33}; return; }
         |  'chocolate3'i           %{ *this = {205, 102,  29}; return; }
         |  'chocolate4'i           %{ *this = {139,  69,  19}; return; }
         |  'chocolate'i            %{ *this = {210, 105,  30}; return; }
         |  'coral1'i               %{ *this = {255, 114,  86}; return; }
         |  'coral2'i               %{ *this = {238, 106,  80}; return; }
         |  'coral3'i               %{ *this = {205,  91,  69}; return; }
         |  'coral4'i               %{ *this = {139,  62,  47}; return; }
         |  'coral'i                %{ *this = {255, 127,  80}; return; }
         |  'cornflowerblue'i       %{ *this = {100, 149, 237}; return; }
         |  'cornsilk1'i            %{ *this = {255, 248, 220}; return; }
         |  'cornsilk2'i            %{ *this = {238, 232, 205}; return; }
         |  'cornsilk3'i            %{ *this = {205, 200, 177}; return; }
         |  'cornsilk4'i            %{ *this = {139, 136, 120}; return; }
         |  'cornsilk'i             %{ *this = {255, 248, 220}; return; }
         |  'cyan1'i                %{ *this = {  0, 255, 255}; return; }
         |  'cyan2'i                %{ *this = {  0, 238, 238}; return; }
         |  'cyan3'i                %{ *this = {  0, 205, 205}; return; }
         |  'cyan4'i                %{ *this = {  0, 139, 139}; return; }
         |  'cyan'i                 %{ *this = {  0, 255, 255}; return; }
         |  'darkblue'i             %{ *this = {0,     0, 139}; return; }
         |  'darkcyan'i             %{ *this = {0,   139, 139}; return; }
         |  'darkgoldenrod1'i       %{ *this = {255, 185,  15}; return; }
         |  'darkgoldenrod2'i       %{ *this = {238, 173,  14}; return; }
         |  'darkgoldenrod3'i       %{ *this = {205, 149,  12}; return; }
         |  'darkgoldenrod4'i       %{ *this = {139, 101,   8}; return; }
         |  'darkgoldenrod'i        %{ *this = {184, 134,  11}; return; }
         |  'darkgray'i             %{ *this = {169, 169, 169}; return; }
         |  'darkgreen'i            %{ *this = {  0, 100,   0}; return; }
         |  'darkgrey'i             %{ *this = {169, 169, 169}; return; }
         |  'darkkhaki'i            %{ *this = {189, 183, 107}; return; }
         |  'darkmagenta'i          %{ *this = {139,   0, 139}; return; }
         |  'darkolivegreen1'i      %{ *this = {202, 255, 112}; return; }
         |  'darkolivegreen2'i      %{ *this = {188, 238, 104}; return; }
         |  'darkolivegreen3'i      %{ *this = {162, 205,  90}; return; }
         |  'darkolivegreen4'i      %{ *this = {110, 139,  61}; return; }
         |  'darkolivegreen'i       %{ *this = { 85, 107,  47}; return; }
         |  'darkorange1'i          %{ *this = {255, 127,   0}; return; }
         |  'darkorange2'i          %{ *this = {238, 118,   0}; return; }
         |  'darkorange3'i          %{ *this = {205, 102,   0}; return; }
         |  'darkorange4'i          %{ *this = {139,  69,   0}; return; }
         |  'darkorange'i           %{ *this = {255, 140,   0}; return; }
         |  'darkorchid1'i          %{ *this = {191,  62, 255}; return; }
         |  'darkorchid2'i          %{ *this = {178,  58, 238}; return; }
         |  'darkorchid3'i          %{ *this = {154,  50, 205}; return; }
         |  'darkorchid4'i          %{ *this = {104,  34, 139}; return; }
         |  'darkorchid'i           %{ *this = {153,  50, 204}; return; }
         |  'darkred'i              %{ *this = {139,   0,   0}; return; }
         |  'darksalmon'i           %{ *this = {233, 150, 122}; return; }
         |  'darkseagreen1'i        %{ *this = {193, 255, 193}; return; }
         |  'darkseagreen2'i        %{ *this = {180, 238, 180}; return; }
         |  'darkseagreen3'i        %{ *this = {155, 205, 155}; return; }
         |  'darkseagreen4'i        %{ *this = {105, 139, 105}; return; }
         |  'darkseagreen'i         %{ *this = {143, 188, 143}; return; }
         |  'darkslateblue'i        %{ *this = { 72,  61, 139}; return; }
         |  'darkslategray1'i       %{ *this = {151, 255, 255}; return; }
         |  'darkslategray2'i       %{ *this = {141, 238, 238}; return; }
         |  'darkslategray3'i       %{ *this = {121, 205, 205}; return; }
         |  'darkslategray4'i       %{ *this = { 82, 139, 139}; return; }
         |  'darkslategray'i        %{ *this = { 47,  79,  79}; return; }
         |  'darkslategrey'i        %{ *this = { 47,  79,  79}; return; }
         |  'darkturquoise'i        %{ *this = {  0, 206, 209}; return; }
         |  'darkviolet'i           %{ *this = {148,   0, 211}; return; }
         |  'deeppink1'i            %{ *this = {255,  20, 147}; return; }
         |  'deeppink2'i            %{ *this = {238,  18, 137}; return; }
         |  'deeppink3'i            %{ *this = {205,  16, 118}; return; }
         |  'deeppink4'i            %{ *this = {139,  10,  80}; return; }
         |  'deeppink'i             %{ *this = {255,  20, 147}; return; }
         |  'deepskyblue1'i         %{ *this = {  0, 191, 255}; return; }
         |  'deepskyblue2'i         %{ *this = {  0, 178, 238}; return; }
         |  'deepskyblue3'i         %{ *this = {  0, 154, 205}; return; }
         |  'deepskyblue4'i         %{ *this = {  0, 104, 139}; return; }
         |  'deepskyblue'i          %{ *this = {  0, 191, 255}; return; }
         |  'dimgray'i              %{ *this = {105, 105, 105}; return; }
         |  'dimgrey'i              %{ *this = {105, 105, 105}; return; }
         |  'dodgerblue1'i          %{ *this = { 30, 144, 255}; return; }
         |  'dodgerblue2'i          %{ *this = { 28, 134, 238}; return; }
         |  'dodgerblue3'i          %{ *this = { 24, 116, 205}; return; }
         |  'dodgerblue4'i          %{ *this = { 16,  78, 139}; return; }
         |  'dodgerblue'i           %{ *this = { 30, 144, 255}; return; }
         |  'firebrick1'i           %{ *this = {255,  48,  48}; return; }
         |  'firebrick2'i           %{ *this = {238,  44,  44}; return; }
         |  'firebrick3'i           %{ *this = {205,  38,  38}; return; }
         |  'firebrick4'i           %{ *this = {139,  26,  26}; return; }
         |  'firebrick'i            %{ *this = {178,  34,  34}; return; }
         |  'floralwhite'i          %{ *this = {255, 250, 240}; return; }
         |  'forestgreen'i          %{ *this = { 34, 139,  34}; return; }
         |  'gainsboro'i            %{ *this = {220, 220, 220}; return; }
         |  'ghostwhite'i           %{ *this = {248, 248, 255}; return; }
         |  'gold1'i                %{ *this = {255, 215,   0}; return; }
         |  'gold2'i                %{ *this = {238, 201,   0}; return; }
         |  'gold3'i                %{ *this = {205, 173,   0}; return; }
         |  'gold4'i                %{ *this = {139, 117,   0}; return; }
         |  'goldenrod1'i           %{ *this = {255, 193,  37}; return; }
         |  'goldenrod2'i           %{ *this = {238, 180,  34}; return; }
         |  'goldenrod3'i           %{ *this = {205, 155,  29}; return; }
         |  'goldenrod4'i           %{ *this = {139, 105,  20}; return; }
         |  'goldenrod'i            %{ *this = {218, 165,  32}; return; }
         |  'gold'i                 %{ *this = {255, 215,   0}; return; }
         |  'gray0'i                %{ *this = {  0,   0,   0}; return; }
         |  'gray100'i              %{ *this = {255, 255, 255}; return; }
         |  'gray10'i               %{ *this = { 26,  26,  26}; return; }
         |  'gray11'i               %{ *this = { 28,  28,  28}; return; }
         |  'gray12'i               %{ *this = { 31,  31,  31}; return; }
         |  'gray13'i               %{ *this = { 33,  33,  33}; return; }
         |  'gray14'i               %{ *this = { 36,  36,  36}; return; }
         |  'gray15'i               %{ *this = { 38,  38,  38}; return; }
         |  'gray16'i               %{ *this = { 41,  41,  41}; return; }
         |  'gray17'i               %{ *this = { 43,  43,  43}; return; }
         |  'gray18'i               %{ *this = { 46,  46,  46}; return; }
         |  'gray19'i               %{ *this = { 48,  48,  48}; return; }
         |  'gray1'i                %{ *this = {  3,   3,   3}; return; }
         |  'gray20'i               %{ *this = { 51,  51,  51}; return; }
         |  'gray21'i               %{ *this = { 54,  54,  54}; return; }
         |  'gray22'i               %{ *this = { 56,  56,  56}; return; }
         |  'gray23'i               %{ *this = { 59,  59,  59}; return; }
         |  'gray24'i               %{ *this = { 61,  61,  61}; return; }
         |  'gray25'i               %{ *this = { 64,  64,  64}; return; }
         |  'gray26'i               %{ *this = { 66,  66,  66}; return; }
         |  'gray27'i               %{ *this = { 69,  69,  69}; return; }
         |  'gray28'i               %{ *this = { 71,  71,  71}; return; }
         |  'gray29'i               %{ *this = { 74,  74,  74}; return; }
         |  'gray2'i                %{ *this = {  5,   5,   5}; return; }
         |  'gray30'i               %{ *this = { 77,  77,  77}; return; }
         |  'gray31'i               %{ *this = { 79,  79,  79}; return; }
         |  'gray32'i               %{ *this = { 82,  82,  82}; return; }
         |  'gray33'i               %{ *this = { 84,  84,  84}; return; }
         |  'gray34'i               %{ *this = { 87,  87,  87}; return; }
         |  'gray35'i               %{ *this = { 89,  89,  89}; return; }
         |  'gray36'i               %{ *this = { 92,  92,  92}; return; }
         |  'gray37'i               %{ *this = { 94,  94,  94}; return; }
         |  'gray38'i               %{ *this = { 97,  97,  97}; return; }
         |  'gray39'i               %{ *this = { 99,  99,  99}; return; }
         |  'gray3'i                %{ *this = {  8,   8,   8}; return; }
         |  'gray40'i               %{ *this = {102, 102, 102}; return; }
         |  'gray41'i               %{ *this = {105, 105, 105}; return; }
         |  'gray42'i               %{ *this = {107, 107, 107}; return; }
         |  'gray43'i               %{ *this = {110, 110, 110}; return; }
         |  'gray44'i               %{ *this = {112, 112, 112}; return; }
         |  'gray45'i               %{ *this = {115, 115, 115}; return; }
         |  'gray46'i               %{ *this = {117, 117, 117}; return; }
         |  'gray47'i               %{ *this = {120, 120, 120}; return; }
         |  'gray48'i               %{ *this = {122, 122, 122}; return; }
         |  'gray49'i               %{ *this = {125, 125, 125}; return; }
         |  'gray4'i                %{ *this = { 10,  10,  10}; return; }
         |  'gray50'i               %{ *this = {127, 127, 127}; return; }
         |  'gray51'i               %{ *this = {130, 130, 130}; return; }
         |  'gray52'i               %{ *this = {133, 133, 133}; return; }
         |  'gray53'i               %{ *this = {135, 135, 135}; return; }
         |  'gray54'i               %{ *this = {138, 138, 138}; return; }
         |  'gray55'i               %{ *this = {140, 140, 140}; return; }
         |  'gray56'i               %{ *this = {143, 143, 143}; return; }
         |  'gray57'i               %{ *this = {145, 145, 145}; return; }
         |  'gray58'i               %{ *this = {148, 148, 148}; return; }
         |  'gray59'i               %{ *this = {150, 150, 150}; return; }
         |  'gray5'i                %{ *this = { 13,  13,  13}; return; }
         |  'gray60'i               %{ *this = {153, 153, 153}; return; }
         |  'gray61'i               %{ *this = {156, 156, 156}; return; }
         |  'gray62'i               %{ *this = {158, 158, 158}; return; }
         |  'gray63'i               %{ *this = {161, 161, 161}; return; }
         |  'gray64'i               %{ *this = {163, 163, 163}; return; }
         |  'gray65'i               %{ *this = {166, 166, 166}; return; }
         |  'gray66'i               %{ *this = {168, 168, 168}; return; }
         |  'gray67'i               %{ *this = {171, 171, 171}; return; }
         |  'gray68'i               %{ *this = {173, 173, 173}; return; }
         |  'gray69'i               %{ *this = {176, 176, 176}; return; }
         |  'gray6'i                %{ *this = { 15,  15,  15}; return; }
         |  'gray70'i               %{ *this = {179, 179, 179}; return; }
         |  'gray71'i               %{ *this = {181, 181, 181}; return; }
         |  'gray72'i               %{ *this = {184, 184, 184}; return; }
         |  'gray73'i               %{ *this = {186, 186, 186}; return; }
         |  'gray74'i               %{ *this = {189, 189, 189}; return; }
         |  'gray75'i               %{ *this = {191, 191, 191}; return; }
         |  'gray76'i               %{ *this = {194, 194, 194}; return; }
         |  'gray77'i               %{ *this = {196, 196, 196}; return; }
         |  'gray78'i               %{ *this = {199, 199, 199}; return; }
         |  'gray79'i               %{ *this = {201, 201, 201}; return; }
         |  'gray7'i                %{ *this = { 18,  18,  18}; return; }
         |  'gray80'i               %{ *this = {204, 204, 204}; return; }
         |  'gray81'i               %{ *this = {207, 207, 207}; return; }
         |  'gray82'i               %{ *this = {209, 209, 209}; return; }
         |  'gray83'i               %{ *this = {212, 212, 212}; return; }
         |  'gray84'i               %{ *this = {214, 214, 214}; return; }
         |  'gray85'i               %{ *this = {217, 217, 217}; return; }
         |  'gray86'i               %{ *this = {219, 219, 219}; return; }
         |  'gray87'i               %{ *this = {222, 222, 222}; return; }
         |  'gray88'i               %{ *this = {224, 224, 224}; return; }
         |  'gray89'i               %{ *this = {227, 227, 227}; return; }
         |  'gray8'i                %{ *this = { 20,  20,  20}; return; }
         |  'gray90'i               %{ *this = {229, 229, 229}; return; }
         |  'gray91'i               %{ *this = {232, 232, 232}; return; }
         |  'gray92'i               %{ *this = {235, 235, 235}; return; }
         |  'gray93'i               %{ *this = {237, 237, 237}; return; }
         |  'gray94'i               %{ *this = {240, 240, 240}; return; }
         |  'gray95'i               %{ *this = {242, 242, 242}; return; }
         |  'gray96'i               %{ *this = {245, 245, 245}; return; }
         |  'gray97'i               %{ *this = {247, 247, 247}; return; }
         |  'gray98'i               %{ *this = {250, 250, 250}; return; }
         |  'gray99'i               %{ *this = {252, 252, 252}; return; }
         |  'gray9'i                %{ *this = { 23,  23,  23}; return; }
         |  'gray'i                 %{ *this = {190, 190, 190}; return; }
         |  'green1'i               %{ *this = {  0, 255,   0}; return; }
         |  'green2'i               %{ *this = {  0, 238,   0}; return; }
         |  'green3'i               %{ *this = {  0, 205,   0}; return; }
         |  'green4'i               %{ *this = {  0, 139,   0}; return; }
         |  'green'i                %{ *this = {  0, 255,   0}; return; }
         |  'greenyellow'i          %{ *this = {173, 255,  47}; return; }
         |  'grey0'i                %{ *this = {  0,   0,   0}; return; }
         |  'grey100'i              %{ *this = {255, 255, 255}; return; }
         |  'grey10'i               %{ *this = { 26,  26,  26}; return; }
         |  'grey11'i               %{ *this = { 28,  28,  28}; return; }
         |  'grey12'i               %{ *this = { 31,  31,  31}; return; }
         |  'grey13'i               %{ *this = { 33,  33,  33}; return; }
         |  'grey14'i               %{ *this = { 36,  36,  36}; return; }
         |  'grey15'i               %{ *this = { 38,  38,  38}; return; }
         |  'grey16'i               %{ *this = { 41,  41,  41}; return; }
         |  'grey17'i               %{ *this = { 43,  43,  43}; return; }
         |  'grey18'i               %{ *this = { 46,  46,  46}; return; }
         |  'grey19'i               %{ *this = { 48,  48,  48}; return; }
         |  'grey1'i                %{ *this = {  3,   3,   3}; return; }
         |  'grey20'i               %{ *this = { 51,  51,  51}; return; }
         |  'grey21'i               %{ *this = { 54,  54,  54}; return; }
         |  'grey22'i               %{ *this = { 56,  56,  56}; return; }
         |  'grey23'i               %{ *this = { 59,  59,  59}; return; }
         |  'grey24'i               %{ *this = { 61,  61,  61}; return; }
         |  'grey25'i               %{ *this = { 64,  64,  64}; return; }
         |  'grey26'i               %{ *this = { 66,  66,  66}; return; }
         |  'grey27'i               %{ *this = { 69,  69,  69}; return; }
         |  'grey28'i               %{ *this = { 71,  71,  71}; return; }
         |  'grey29'i               %{ *this = { 74,  74,  74}; return; }
         |  'grey2'i                %{ *this = {  5,   5,   5}; return; }
         |  'grey30'i               %{ *this = { 77,  77,  77}; return; }
         |  'grey31'i               %{ *this = { 79,  79,  79}; return; }
         |  'grey32'i               %{ *this = { 82,  82,  82}; return; }
         |  'grey33'i               %{ *this = { 84,  84,  84}; return; }
         |  'grey34'i               %{ *this = { 87,  87,  87}; return; }
         |  'grey35'i               %{ *this = { 89,  89,  89}; return; }
         |  'grey36'i               %{ *this = { 92,  92,  92}; return; }
         |  'grey37'i               %{ *this = { 94,  94,  94}; return; }
         |  'grey38'i               %{ *this = { 97,  97,  97}; return; }
         |  'grey39'i               %{ *this = { 99,  99,  99}; return; }
         |  'grey3'i                %{ *this = {  8,   8,   8}; return; }
         |  'grey40'i               %{ *this = {102, 102, 102}; return; }
         |  'grey41'i               %{ *this = {105, 105, 105}; return; }
         |  'grey42'i               %{ *this = {107, 107, 107}; return; }
         |  'grey43'i               %{ *this = {110, 110, 110}; return; }
         |  'grey44'i               %{ *this = {112, 112, 112}; return; }
         |  'grey45'i               %{ *this = {115, 115, 115}; return; }
         |  'grey46'i               %{ *this = {117, 117, 117}; return; }
         |  'grey47'i               %{ *this = {120, 120, 120}; return; }
         |  'grey48'i               %{ *this = {122, 122, 122}; return; }
         |  'grey49'i               %{ *this = {125, 125, 125}; return; }
         |  'grey4'i                %{ *this = { 10,  10,  10}; return; }
         |  'grey50'i               %{ *this = {127, 127, 127}; return; }
         |  'grey51'i               %{ *this = {130, 130, 130}; return; }
         |  'grey52'i               %{ *this = {133, 133, 133}; return; }
         |  'grey53'i               %{ *this = {135, 135, 135}; return; }
         |  'grey54'i               %{ *this = {138, 138, 138}; return; }
         |  'grey55'i               %{ *this = {140, 140, 140}; return; }
         |  'grey56'i               %{ *this = {143, 143, 143}; return; }
         |  'grey57'i               %{ *this = {145, 145, 145}; return; }
         |  'grey58'i               %{ *this = {148, 148, 148}; return; }
         |  'grey59'i               %{ *this = {150, 150, 150}; return; }
         |  'grey5'i                %{ *this = { 13,  13,  13}; return; }
         |  'grey60'i               %{ *this = {153, 153, 153}; return; }
         |  'grey61'i               %{ *this = {156, 156, 156}; return; }
         |  'grey62'i               %{ *this = {158, 158, 158}; return; }
         |  'grey63'i               %{ *this = {161, 161, 161}; return; }
         |  'grey64'i               %{ *this = {163, 163, 163}; return; }
         |  'grey65'i               %{ *this = {166, 166, 166}; return; }
         |  'grey66'i               %{ *this = {168, 168, 168}; return; }
         |  'grey67'i               %{ *this = {171, 171, 171}; return; }
         |  'grey68'i               %{ *this = {173, 173, 173}; return; }
         |  'grey69'i               %{ *this = {176, 176, 176}; return; }
         |  'grey6'i                %{ *this = { 15,  15,  15}; return; }
         |  'grey70'i               %{ *this = {179, 179, 179}; return; }
         |  'grey71'i               %{ *this = {181, 181, 181}; return; }
         |  'grey72'i               %{ *this = {184, 184, 184}; return; }
         |  'grey73'i               %{ *this = {186, 186, 186}; return; }
         |  'grey74'i               %{ *this = {189, 189, 189}; return; }
         |  'grey75'i               %{ *this = {191, 191, 191}; return; }
         |  'grey76'i               %{ *this = {194, 194, 194}; return; }
         |  'grey77'i               %{ *this = {196, 196, 196}; return; }
         |  'grey78'i               %{ *this = {199, 199, 199}; return; }
         |  'grey79'i               %{ *this = {201, 201, 201}; return; }
         |  'grey7'i                %{ *this = { 18,  18,  18}; return; }
         |  'grey80'i               %{ *this = {204, 204, 204}; return; }
         |  'grey81'i               %{ *this = {207, 207, 207}; return; }
         |  'grey82'i               %{ *this = {209, 209, 209}; return; }
         |  'grey83'i               %{ *this = {212, 212, 212}; return; }
         |  'grey84'i               %{ *this = {214, 214, 214}; return; }
         |  'grey85'i               %{ *this = {217, 217, 217}; return; }
         |  'grey86'i               %{ *this = {219, 219, 219}; return; }
         |  'grey87'i               %{ *this = {222, 222, 222}; return; }
         |  'grey88'i               %{ *this = {224, 224, 224}; return; }
         |  'grey89'i               %{ *this = {227, 227, 227}; return; }
         |  'grey8'i                %{ *this = { 20,  20,  20}; return; }
         |  'grey90'i               %{ *this = {229, 229, 229}; return; }
         |  'grey91'i               %{ *this = {232, 232, 232}; return; }
         |  'grey92'i               %{ *this = {235, 235, 235}; return; }
         |  'grey93'i               %{ *this = {237, 237, 237}; return; }
         |  'grey94'i               %{ *this = {240, 240, 240}; return; }
         |  'grey95'i               %{ *this = {242, 242, 242}; return; }
         |  'grey96'i               %{ *this = {245, 245, 245}; return; }
         |  'grey97'i               %{ *this = {247, 247, 247}; return; }
         |  'grey98'i               %{ *this = {250, 250, 250}; return; }
         |  'grey99'i               %{ *this = {252, 252, 252}; return; }
         |  'grey9'i                %{ *this = { 23,  23,  23}; return; }
         |  'grey'i                 %{ *this = {190, 190, 190}; return; }
         |  'honeydew1'i            %{ *this = {240, 255, 240}; return; }
         |  'honeydew2'i            %{ *this = {224, 238, 224}; return; }
         |  'honeydew3'i            %{ *this = {193, 205, 193}; return; }
         |  'honeydew4'i            %{ *this = {131, 139, 131}; return; }
         |  'honeydew'i             %{ *this = {240, 255, 240}; return; }
         |  'hotpink1'i             %{ *this = {255, 110, 180}; return; }
         |  'hotpink2'i             %{ *this = {238, 106, 167}; return; }
         |  'hotpink3'i             %{ *this = {205,  96, 144}; return; }
         |  'hotpink4'i             %{ *this = {139,  58,  98}; return; }
         |  'hotpink'i              %{ *this = {255, 105, 180}; return; }
         |  'indianred1'i           %{ *this = {255, 106, 106}; return; }
         |  'indianred2'i           %{ *this = {238,  99,  99}; return; }
         |  'indianred3'i           %{ *this = {205,  85,  85}; return; }
         |  'indianred4'i           %{ *this = {139,  58,  58}; return; }
         |  'indianred'i            %{ *this = {205,  92,  92}; return; }
         |  'ivory1'i               %{ *this = {255, 255, 240}; return; }
         |  'ivory2'i               %{ *this = {238, 238, 224}; return; }
         |  'ivory3'i               %{ *this = {205, 205, 193}; return; }
         |  'ivory4'i               %{ *this = {139, 139, 131}; return; }
         |  'ivory'i                %{ *this = {255, 255, 240}; return; }
         |  'khaki1'i               %{ *this = {255, 246, 143}; return; }
         |  'khaki2'i               %{ *this = {238, 230, 133}; return; }
         |  'khaki3'i               %{ *this = {205, 198, 115}; return; }
         |  'khaki4'i               %{ *this = {139, 134,  78}; return; }
         |  'khaki'i                %{ *this = {240, 230, 140}; return; }
         |  'lavenderblush1'i       %{ *this = {255, 240, 245}; return; }
         |  'lavenderblush2'i       %{ *this = {238, 224, 229}; return; }
         |  'lavenderblush3'i       %{ *this = {205, 193, 197}; return; }
         |  'lavenderblush4'i       %{ *this = {139, 131, 134}; return; }
         |  'lavenderblush'i        %{ *this = {255, 240, 245}; return; }
         |  'lavender'i             %{ *this = {230, 230, 250}; return; }
         |  'lawngreen'i            %{ *this = {124, 252,   0}; return; }
         |  'lemonchiffon1'i        %{ *this = {255, 250, 205}; return; }
         |  'lemonchiffon2'i        %{ *this = {238, 233, 191}; return; }
         |  'lemonchiffon3'i        %{ *this = {205, 201, 165}; return; }
         |  'lemonchiffon4'i        %{ *this = {139, 137, 112}; return; }
         |  'lemonchiffon'i         %{ *this = {255, 250, 205}; return; }
         |  'lightblue1'i           %{ *this = {191, 239, 255}; return; }
         |  'lightblue2'i           %{ *this = {178, 223, 238}; return; }
         |  'lightblue3'i           %{ *this = {154, 192, 205}; return; }
         |  'lightblue4'i           %{ *this = {104, 131, 139}; return; }
         |  'lightblue'i            %{ *this = {173, 216, 230}; return; }
         |  'lightcoral'i           %{ *this = {240, 128, 128}; return; }
         |  'lightcyan1'i           %{ *this = {224, 255, 255}; return; }
         |  'lightcyan2'i           %{ *this = {209, 238, 238}; return; }
         |  'lightcyan3'i           %{ *this = {180, 205, 205}; return; }
         |  'lightcyan4'i           %{ *this = {122, 139, 139}; return; }
         |  'lightcyan'i            %{ *this = {224, 255, 255}; return; }
         |  'lightgoldenrod1'i      %{ *this = {255, 236, 139}; return; }
         |  'lightgoldenrod2'i      %{ *this = {238, 220, 130}; return; }
         |  'lightgoldenrod3'i      %{ *this = {205, 190, 112}; return; }
         |  'lightgoldenrod4'i      %{ *this = {139, 129,  76}; return; }
         |  'lightgoldenrod'i       %{ *this = {238, 221, 130}; return; }
         |  'lightgoldenrodyellow'i %{ *this = {250, 250, 210}; return; }
         |  'lightgray'i            %{ *this = {211, 211, 211}; return; }
         |  'lightgreen'i           %{ *this = {144, 238, 144}; return; }
         |  'lightgrey'i            %{ *this = {211, 211, 211}; return; }
         |  'lightpink1'i           %{ *this = {255, 174, 185}; return; }
         |  'lightpink2'i           %{ *this = {238, 162, 173}; return; }
         |  'lightpink3'i           %{ *this = {205, 140, 149}; return; }
         |  'lightpink4'i           %{ *this = {139,  95, 101}; return; }
         |  'lightpink'i            %{ *this = {255, 182, 193}; return; }
         |  'lightsalmon1'i         %{ *this = {255, 160, 122}; return; }
         |  'lightsalmon2'i         %{ *this = {238, 149, 114}; return; }
         |  'lightsalmon3'i         %{ *this = {205, 129,  98}; return; }
         |  'lightsalmon4'i         %{ *this = {139,  87,  66}; return; }
         |  'lightsalmon'i          %{ *this = {255, 160, 122}; return; }
         |  'lightseagreen'i        %{ *this = { 32, 178, 170}; return; }
         |  'lightskyblue1'i        %{ *this = {176, 226, 255}; return; }
         |  'lightskyblue2'i        %{ *this = {164, 211, 238}; return; }
         |  'lightskyblue3'i        %{ *this = {141, 182, 205}; return; }
         |  'lightskyblue4'i        %{ *this = { 96, 123, 139}; return; }
         |  'lightskyblue'i         %{ *this = {135, 206, 250}; return; }
         |  'lightslateblue'i       %{ *this = {132, 112, 255}; return; }
         |  'lightslategray'i       %{ *this = {119, 136, 153}; return; }
         |  'lightslategrey'i       %{ *this = {119, 136, 153}; return; }
         |  'lightsteelblue1'i      %{ *this = {202, 225, 255}; return; }
         |  'lightsteelblue2'i      %{ *this = {188, 210, 238}; return; }
         |  'lightsteelblue3'i      %{ *this = {162, 181, 205}; return; }
         |  'lightsteelblue4'i      %{ *this = {110, 123, 139}; return; }
         |  'lightsteelblue'i       %{ *this = {176, 196, 222}; return; }
         |  'lightyellow1'i         %{ *this = {255, 255, 224}; return; }
         |  'lightyellow2'i         %{ *this = {238, 238, 209}; return; }
         |  'lightyellow3'i         %{ *this = {205, 205, 180}; return; }
         |  'lightyellow4'i         %{ *this = {139, 139, 122}; return; }
         |  'lightyellow'i          %{ *this = {255, 255, 224}; return; }
         |  'limegreen'i            %{ *this = { 50, 205,  50}; return; }
         |  'linen'i                %{ *this = {250, 240, 230}; return; }
         |  'magenta1'i             %{ *this = {255,   0, 255}; return; }
         |  'magenta2'i             %{ *this = {238,   0, 238}; return; }
         |  'magenta3'i             %{ *this = {205,   0, 205}; return; }
         |  'magenta4'i             %{ *this = {139,   0, 139}; return; }
         |  'magenta'i              %{ *this = {255,   0, 255}; return; }
         |  'maroon1'i              %{ *this = {255,  52, 179}; return; }
         |  'maroon2'i              %{ *this = {238,  48, 167}; return; }
         |  'maroon3'i              %{ *this = {205,  41, 144}; return; }
         |  'maroon4'i              %{ *this = {139,  28,  98}; return; }
         |  'maroon'i               %{ *this = {176,  48,  96}; return; }
         |  'mediumaquamarine'i     %{ *this = {102, 205, 170}; return; }
         |  'mediumblue'i           %{ *this = {  0,   0, 205}; return; }
         |  'mediumorchid1'i        %{ *this = {224, 102, 255}; return; }
         |  'mediumorchid2'i        %{ *this = {209,  95, 238}; return; }
         |  'mediumorchid3'i        %{ *this = {180,  82, 205}; return; }
         |  'mediumorchid4'i        %{ *this = {122,  55, 139}; return; }
         |  'mediumorchid'i         %{ *this = {186,  85, 211}; return; }
         |  'mediumpurple1'i        %{ *this = {171, 130, 255}; return; }
         |  'mediumpurple2'i        %{ *this = {159, 121, 238}; return; }
         |  'mediumpurple3'i        %{ *this = {137, 104, 205}; return; }
         |  'mediumpurple4'i        %{ *this = { 93,  71, 139}; return; }
         |  'mediumpurple'i         %{ *this = {147, 112, 219}; return; }
         |  'mediumseagreen'i       %{ *this = { 60, 179, 113}; return; }
         |  'mediumslateblue'i      %{ *this = {123, 104, 238}; return; }
         |  'mediumspringgreen'i    %{ *this = {  0, 250, 154}; return; }
         |  'mediumturquoise'i      %{ *this = { 72, 209, 204}; return; }
         |  'mediumvioletred'i      %{ *this = {199,  21, 133}; return; }
         |  'midnightblue'i         %{ *this = { 25,  25, 112}; return; }
         |  'mintcream'i            %{ *this = {245, 255, 250}; return; }
         |  'mistyrose1'i           %{ *this = {255, 228, 225}; return; }
         |  'mistyrose2'i           %{ *this = {238, 213, 210}; return; }
         |  'mistyrose3'i           %{ *this = {205, 183, 181}; return; }
         |  'mistyrose4'i           %{ *this = {139, 125, 123}; return; }
         |  'mistyrose'i            %{ *this = {255, 228, 225}; return; }
         |  'moccasin'i             %{ *this = {255, 228, 181}; return; }
         |  'navajowhite1'i         %{ *this = {255, 222, 173}; return; }
         |  'navajowhite2'i         %{ *this = {238, 207, 161}; return; }
         |  'navajowhite3'i         %{ *this = {205, 179, 139}; return; }
         |  'navajowhite4'i         %{ *this = {139, 121,  94}; return; }
         |  'navajowhite'i          %{ *this = {255, 222, 173}; return; }
         |  'navyblue'i             %{ *this = {  0,   0, 128}; return; }
         |  'navy'i                 %{ *this = {  0,   0, 128}; return; }
         |  'oldlace'i              %{ *this = {253, 245, 230}; return; }
         |  'olivedrab1'i           %{ *this = {192, 255,  62}; return; }
         |  'olivedrab2'i           %{ *this = {179, 238,  58}; return; }
         |  'olivedrab3'i           %{ *this = {154, 205,  50}; return; }
         |  'olivedrab4'i           %{ *this = {105, 139,  34}; return; }
         |  'olivedrab'i            %{ *this = {107, 142,  35}; return; }
         |  'orange1'i              %{ *this = {255, 165,   0}; return; }
         |  'orange2'i              %{ *this = {238, 154,   0}; return; }
         |  'orange3'i              %{ *this = {205, 133,   0}; return; }
         |  'orange4'i              %{ *this = {139,  90,   0}; return; }
         |  'orange'i               %{ *this = {255, 165,   0}; return; }
         |  'orangered1'i           %{ *this = {255,  69,   0}; return; }
         |  'orangered2'i           %{ *this = {238,  64,   0}; return; }
         |  'orangered3'i           %{ *this = {205,  55,   0}; return; }
         |  'orangered4'i           %{ *this = {139,  37,   0}; return; }
         |  'orangered'i            %{ *this = {255,  69,   0}; return; }
         |  'orchid1'i              %{ *this = {255, 131, 250}; return; }
         |  'orchid2'i              %{ *this = {238, 122, 233}; return; }
         |  'orchid3'i              %{ *this = {205, 105, 201}; return; }
         |  'orchid4'i              %{ *this = {139,  71, 137}; return; }
         |  'orchid'i               %{ *this = {218, 112, 214}; return; }
         |  'palegoldenrod'i        %{ *this = {238, 232, 170}; return; }
         |  'palegreen1'i           %{ *this = {154, 255, 154}; return; }
         |  'palegreen2'i           %{ *this = {144, 238, 144}; return; }
         |  'palegreen3'i           %{ *this = {124, 205, 124}; return; }
         |  'palegreen4'i           %{ *this = { 84, 139,  84}; return; }
         |  'palegreen'i            %{ *this = {152, 251, 152}; return; }
         |  'paleturquoise1'i       %{ *this = {187, 255, 255}; return; }
         |  'paleturquoise2'i       %{ *this = {174, 238, 238}; return; }
         |  'paleturquoise3'i       %{ *this = {150, 205, 205}; return; }
         |  'paleturquoise4'i       %{ *this = {102, 139, 139}; return; }
         |  'paleturquoise'i        %{ *this = {175, 238, 238}; return; }
         |  'palevioletred1'i       %{ *this = {255, 130, 171}; return; }
         |  'palevioletred2'i       %{ *this = {238, 121, 159}; return; }
         |  'palevioletred3'i       %{ *this = {205, 104, 137}; return; }
         |  'palevioletred4'i       %{ *this = {139,  71,  93}; return; }
         |  'palevioletred'i        %{ *this = {219, 112, 147}; return; }
         |  'papayawhip'i           %{ *this = {255, 239, 213}; return; }
         |  'peachpuff1'i           %{ *this = {255, 218, 185}; return; }
         |  'peachpuff2'i           %{ *this = {238, 203, 173}; return; }
         |  'peachpuff3'i           %{ *this = {205, 175, 149}; return; }
         |  'peachpuff4'i           %{ *this = {139, 119, 101}; return; }
         |  'peachpuff'i            %{ *this = {255, 218, 185}; return; }
         |  'peru'i                 %{ *this = {205, 133,  63}; return; }
         |  'pink1'i                %{ *this = {255, 181, 197}; return; }
         |  'pink2'i                %{ *this = {238, 169, 184}; return; }
         |  'pink3'i                %{ *this = {205, 145, 158}; return; }
         |  'pink4'i                %{ *this = {139,  99, 108}; return; }
         |  'pink'i                 %{ *this = {255, 192, 203}; return; }
         |  'plum1'i                %{ *this = {255, 187, 255}; return; }
         |  'plum2'i                %{ *this = {238, 174, 238}; return; }
         |  'plum3'i                %{ *this = {205, 150, 205}; return; }
         |  'plum4'i                %{ *this = {139, 102, 139}; return; }
         |  'plum'i                 %{ *this = {221, 160, 221}; return; }
         |  'powderblue'i           %{ *this = {176, 224, 230}; return; }
         |  'purple1'i              %{ *this = {155,  48, 255}; return; }
         |  'purple2'i              %{ *this = {145,  44, 238}; return; }
         |  'purple3'i              %{ *this = {125,  38, 205}; return; }
         |  'purple4'i              %{ *this = { 85,  26, 139}; return; }
         |  'purple'i               %{ *this = {160,  32, 240}; return; }
         |  'red1'i                 %{ *this = {255,   0,   0}; return; }
         |  'red2'i                 %{ *this = {238,   0,   0}; return; }
         |  'red3'i                 %{ *this = {205,   0,   0}; return; }
         |  'red4'i                 %{ *this = {139,   0,   0}; return; }
         |  'red'i                  %{ *this = {255,   0,   0}; return; }
         |  'rosybrown1'i           %{ *this = {255, 193, 193}; return; }
         |  'rosybrown2'i           %{ *this = {238, 180, 180}; return; }
         |  'rosybrown3'i           %{ *this = {205, 155, 155}; return; }
         |  'rosybrown4'i           %{ *this = {139, 105, 105}; return; }
         |  'rosybrown'i            %{ *this = {188, 143, 143}; return; }
         |  'royalblue1'i           %{ *this = { 72, 118, 255}; return; }
         |  'royalblue2'i           %{ *this = { 67, 110, 238}; return; }
         |  'royalblue3'i           %{ *this = { 58,  95, 205}; return; }
         |  'royalblue4'i           %{ *this = { 39,  64, 139}; return; }
         |  'royalblue'i            %{ *this = { 65, 105, 225}; return; }
         |  'saddlebrown'i          %{ *this = {139,  69,  19}; return; }
         |  'salmon1'i              %{ *this = {255, 140, 105}; return; }
         |  'salmon2'i              %{ *this = {238, 130,  98}; return; }
         |  'salmon3'i              %{ *this = {205, 112,  84}; return; }
         |  'salmon4'i              %{ *this = {139,  76,  57}; return; }
         |  'salmon'i               %{ *this = {250, 128, 114}; return; }
         |  'sandybrown'i           %{ *this = {244, 164,  96}; return; }
         |  'seagreen1'i            %{ *this = { 84, 255, 159}; return; }
         |  'seagreen2'i            %{ *this = { 78, 238, 148}; return; }
         |  'seagreen3'i            %{ *this = { 67, 205, 128}; return; }
         |  'seagreen4'i            %{ *this = { 46, 139,  87}; return; }
         |  'seagreen'i             %{ *this = { 46, 139,  87}; return; }
         |  'seashell1'i            %{ *this = {255, 245, 238}; return; }
         |  'seashell2'i            %{ *this = {238, 229, 222}; return; }
         |  'seashell3'i            %{ *this = {205, 197, 191}; return; }
         |  'seashell4'i            %{ *this = {139, 134, 130}; return; }
         |  'seashell'i             %{ *this = {255, 245, 238}; return; }
         |  'sienna1'i              %{ *this = {255, 130,  71}; return; }
         |  'sienna2'i              %{ *this = {238, 121,  66}; return; }
         |  'sienna3'i              %{ *this = {205, 104,  57}; return; }
         |  'sienna4'i              %{ *this = {139,  71,  38}; return; }
         |  'sienna'i               %{ *this = {160,  82,  45}; return; }
         |  'skyblue1'i             %{ *this = {135, 206, 255}; return; }
         |  'skyblue2'i             %{ *this = {126, 192, 238}; return; }
         |  'skyblue3'i             %{ *this = {108, 166, 205}; return; }
         |  'skyblue4'i             %{ *this = { 74, 112, 139}; return; }
         |  'skyblue'i              %{ *this = {135, 206, 235}; return; }
         |  'slateblue1'i           %{ *this = {131, 111, 255}; return; }
         |  'slateblue2'i           %{ *this = {122, 103, 238}; return; }
         |  'slateblue3'i           %{ *this = {105,  89, 205}; return; }
         |  'slateblue4'i           %{ *this = { 71,  60, 139}; return; }
         |  'slateblue'i            %{ *this = {106,  90, 205}; return; }
         |  'slategray1'i           %{ *this = {198, 226, 255}; return; }
         |  'slategray2'i           %{ *this = {185, 211, 238}; return; }
         |  'slategray3'i           %{ *this = {159, 182, 205}; return; }
         |  'slategray4'i           %{ *this = {108, 123, 139}; return; }
         |  'slategray'i            %{ *this = {112, 128, 144}; return; }
         |  'slategrey'i            %{ *this = {112, 128, 144}; return; }
         |  'snow1'i                %{ *this = {255, 250, 250}; return; }
         |  'snow2'i                %{ *this = {238, 233, 233}; return; }
         |  'snow3'i                %{ *this = {205, 201, 201}; return; }
         |  'snow4'i                %{ *this = {139, 137, 137}; return; }
         |  'snow'i                 %{ *this = {255, 250, 250}; return; }
         |  'springgreen1'i         %{ *this = {  0, 255, 127}; return; }
         |  'springgreen2'i         %{ *this = {  0, 238, 118}; return; }
         |  'springgreen3'i         %{ *this = {  0, 205, 102}; return; }
         |  'springgreen4'i         %{ *this = {  0, 139,  69}; return; }
         |  'springgreen'i          %{ *this = {  0, 255, 127}; return; }
         |  'steelblue1'i           %{ *this = { 99, 184, 255}; return; }
         |  'steelblue2'i           %{ *this = { 92, 172, 238}; return; }
         |  'steelblue3'i           %{ *this = { 79, 148, 205}; return; }
         |  'steelblue4'i           %{ *this = { 54, 100, 139}; return; }
         |  'steelblue'i            %{ *this = { 70, 130, 180}; return; }
         |  'tan1'i                 %{ *this = {255, 165,  79}; return; }
         |  'tan2'i                 %{ *this = {238, 154,  73}; return; }
         |  'tan3'i                 %{ *this = {205, 133,  63}; return; }
         |  'tan4'i                 %{ *this = {139,  90,  43}; return; }
         |  'tan'i                  %{ *this = {210, 180, 140}; return; }
         |  'thistle1'i             %{ *this = {255, 225, 255}; return; }
         |  'thistle2'i             %{ *this = {238, 210, 238}; return; }
         |  'thistle3'i             %{ *this = {205, 181, 205}; return; }
         |  'thistle4'i             %{ *this = {139, 123, 139}; return; }
         |  'thistle'i              %{ *this = {216, 191, 216}; return; }
         |  'tomato1'i              %{ *this = {255,  99,  71}; return; }
         |  'tomato2'i              %{ *this = {238,  92,  66}; return; }
         |  'tomato3'i              %{ *this = {205,  79,  57}; return; }
         |  'tomato4'i              %{ *this = {139,  54,  38}; return; }
         |  'tomato'i               %{ *this = {255,  99,  71}; return; }
         |  'turquoise1'i           %{ *this = {  0, 245, 255}; return; }
         |  'turquoise2'i           %{ *this = {  0, 229, 238}; return; }
         |  'turquoise3'i           %{ *this = {  0, 197, 205}; return; }
         |  'turquoise4'i           %{ *this = {  0, 134, 139}; return; }
         |  'turquoise'i            %{ *this = { 64, 224, 208}; return; }
         |  'violet'i               %{ *this = {238, 130, 238}; return; }
         |  'violetred1'i           %{ *this = {255,  62, 150}; return; }
         |  'violetred2'i           %{ *this = {238,  58, 140}; return; }
         |  'violetred3'i           %{ *this = {205,  50, 120}; return; }
         |  'violetred4'i           %{ *this = {139,  34,  82}; return; }
         |  'violetred'i            %{ *this = {208,  32, 144}; return; }
         |  'wheat1'i               %{ *this = {255, 231, 186}; return; }
         |  'wheat2'i               %{ *this = {238, 216, 174}; return; }
         |  'wheat3'i               %{ *this = {205, 186, 150}; return; }
         |  'wheat4'i               %{ *this = {139, 126, 102}; return; }
         |  'wheat'i                %{ *this = {245, 222, 179}; return; }
         |  'whitesmoke'i           %{ *this = {245, 245, 245}; return; }
         |  'yellow1'i              %{ *this = {255, 255,   0}; return; }
         |  'yellow2'i              %{ *this = {238, 238,   0}; return; }
         |  'yellow3'i              %{ *this = {205, 205,   0}; return; }
         |  'yellow4'i              %{ *this = {139, 139,   0}; return; }
         |  'yellowgreen'i          %{ *this = {154, 205,  50}; return; }
         |  'yellow'i               %{ *this = {255, 255,   0}; return; }
         ;

    write init;
    write exec;
  }%%
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
