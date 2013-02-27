// -*- C++ -*-
// Complete CSS3/X11 Color Parser Implementation

#include "pixel.h"
#include <cstdlib>
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
  return (int)((float)val / 100.0 * 255.0);
}

Pixel Pixel::Parse(const std::string& name) {
  CHECK(name.size() > 0);
  int cs;
  const char* f = &name.front();
  const char* p = f;
  const char* pe = p + name.size();
  const char* eof = pe;
  const char* mark;
  const char* e;
  int val;
  double fval = 1.0;
  std::vector<int> vals;
  %%{
    machine pixel_color;

    action hex_rgb {
      return Pixel(Color256(UnHex(p[-6]) * 16 + UnHex(p[-5])),
                   Color256(UnHex(p[-4]) * 16 + UnHex(p[-3])),
                   Color256(UnHex(p[-2]) * 16 + UnHex(p[-1])),
                   1.0);
    }

    action hex_rgba {
      return Pixel(Color256(UnHex(p[-8]) * 16 + UnHex(p[-7])),
                   Color256(UnHex(p[-6]) * 16 + UnHex(p[-5])),
                   Color256(UnHex(p[-4]) * 16 + UnHex(p[-3])),
                   Color256(UnHex(p[-2]) * 16 + UnHex(p[-1])));
    }

    action hex_rgb_short {
      return Pixel(Color256(UnHex(p[-3]) * 16 + UnHex(p[-3])),
                   Color256(UnHex(p[-2]) * 16 + UnHex(p[-2])),
                   Color256(UnHex(p[-1]) * 16 + UnHex(p[-1])),
                   1.0);
    }

    action rgb {
      DCHECK(vals.size() == 3);
      return Pixel(Color256(vals[0]),
                   Color256(vals[1]),
                   Color256(vals[2]),
                   fval);
    }

    action hsl {
      DCHECK(vals.size() == 3);
      return Pixel::HSL(Color256(vals[0]),
                        Color256(vals[1]),
                        Color256(vals[2]),
                        fval);
    }

    sign = '-' | '+' ;
    real = sign? [.0-9]* ( 'e'i sign? digit+ )? ;
    atof = real >{ mark = p; } %{ e = p; fval = strtod(mark, (char **)&e); } ;
    atoi = digit+ >{ val = 0; } ${ val = val * 10 + (*p - '0'); } ;

    number = '-' atoi  %{ vals.push_back(0); }
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
         |  'white'i                %{ return Pixel::kWhite;         }
         |  'black'i                %{ return Pixel::kBlack;         }
         |  'transparent'i          %{ return Pixel::kClear;         }
         |  'aliceblue'i            %{ return RGB256(240, 248, 255); }
         |  'antiquewhite1'i        %{ return RGB256(255, 239, 219); }
         |  'antiquewhite2'i        %{ return RGB256(238, 223, 204); }
         |  'antiquewhite3'i        %{ return RGB256(205, 192, 176); }
         |  'antiquewhite4'i        %{ return RGB256(139, 131, 120); }
         |  'antiquewhite'i         %{ return RGB256(250, 235, 215); }
         |  'aquamarine1'i          %{ return RGB256(127, 255, 212); }
         |  'aquamarine2'i          %{ return RGB256(118, 238, 198); }
         |  'aquamarine3'i          %{ return RGB256(102, 205, 170); }
         |  'aquamarine4'i          %{ return RGB256( 69, 139, 116); }
         |  'aquamarine'i           %{ return RGB256(127, 255, 212); }
         |  'azure1'i               %{ return RGB256(240, 255, 255); }
         |  'azure2'i               %{ return RGB256(224, 238, 238); }
         |  'azure3'i               %{ return RGB256(193, 205, 205); }
         |  'azure4'i               %{ return RGB256(131, 139, 139); }
         |  'azure'i                %{ return RGB256(240, 255, 255); }
         |  'beige'i                %{ return RGB256(245, 245, 220); }
         |  'bisque1'i              %{ return RGB256(255, 228, 196); }
         |  'bisque2'i              %{ return RGB256(238, 213, 183); }
         |  'bisque3'i              %{ return RGB256(205, 183, 158); }
         |  'bisque4'i              %{ return RGB256(139, 125, 107); }
         |  'bisque'i               %{ return RGB256(255, 228, 196); }
         |  'blanchedalmond'i       %{ return RGB256(255, 235, 205); }
         |  'blue1'i                %{ return RGB256(  0,   0, 255); }
         |  'blue2'i                %{ return RGB256(  0,   0, 238); }
         |  'blue3'i                %{ return RGB256(  0,   0, 205); }
         |  'blue4'i                %{ return RGB256(  0,   0, 139); }
         |  'blue'i                 %{ return RGB256(  0,   0, 255); }
         |  'blueviolet'i           %{ return RGB256(138,  43, 226); }
         |  'brown1'i               %{ return RGB256(255,  64,  64); }
         |  'brown2'i               %{ return RGB256(238,  59,  59); }
         |  'brown3'i               %{ return RGB256(205,  51,  51); }
         |  'brown4'i               %{ return RGB256(139,  35,  35); }
         |  'brown'i                %{ return RGB256(165,  42,  42); }
         |  'burlywood1'i           %{ return RGB256(255, 211, 155); }
         |  'burlywood2'i           %{ return RGB256(238, 197, 145); }
         |  'burlywood3'i           %{ return RGB256(205, 170, 125); }
         |  'burlywood4'i           %{ return RGB256(139, 115,  85); }
         |  'burlywood'i            %{ return RGB256(222, 184, 135); }
         |  'cadetblue1'i           %{ return RGB256(152, 245, 255); }
         |  'cadetblue2'i           %{ return RGB256(142, 229, 238); }
         |  'cadetblue3'i           %{ return RGB256(122, 197, 205); }
         |  'cadetblue4'i           %{ return RGB256( 83, 134, 139); }
         |  'cadetblue'i            %{ return RGB256( 95, 158, 160); }
         |  'chartreuse1'i          %{ return RGB256(127, 255,   0); }
         |  'chartreuse2'i          %{ return RGB256(118, 238,   0); }
         |  'chartreuse3'i          %{ return RGB256(102, 205,   0); }
         |  'chartreuse4'i          %{ return RGB256( 69, 139,   0); }
         |  'chartreuse'i           %{ return RGB256(127, 255,   0); }
         |  'chocolate1'i           %{ return RGB256(255, 127,  36); }
         |  'chocolate2'i           %{ return RGB256(238, 118,  33); }
         |  'chocolate3'i           %{ return RGB256(205, 102,  29); }
         |  'chocolate4'i           %{ return RGB256(139,  69,  19); }
         |  'chocolate'i            %{ return RGB256(210, 105,  30); }
         |  'coral1'i               %{ return RGB256(255, 114,  86); }
         |  'coral2'i               %{ return RGB256(238, 106,  80); }
         |  'coral3'i               %{ return RGB256(205,  91,  69); }
         |  'coral4'i               %{ return RGB256(139,  62,  47); }
         |  'coral'i                %{ return RGB256(255, 127,  80); }
         |  'cornflowerblue'i       %{ return RGB256(100, 149, 237); }
         |  'cornsilk1'i            %{ return RGB256(255, 248, 220); }
         |  'cornsilk2'i            %{ return RGB256(238, 232, 205); }
         |  'cornsilk3'i            %{ return RGB256(205, 200, 177); }
         |  'cornsilk4'i            %{ return RGB256(139, 136, 120); }
         |  'cornsilk'i             %{ return RGB256(255, 248, 220); }
         |  'cyan1'i                %{ return RGB256(  0, 255, 255); }
         |  'cyan2'i                %{ return RGB256(  0, 238, 238); }
         |  'cyan3'i                %{ return RGB256(  0, 205, 205); }
         |  'cyan4'i                %{ return RGB256(  0, 139, 139); }
         |  'cyan'i                 %{ return RGB256(  0, 255, 255); }
         |  'darkblue'i             %{ return RGB256(0,     0, 139); }
         |  'darkcyan'i             %{ return RGB256(0,   139, 139); }
         |  'darkgoldenrod1'i       %{ return RGB256(255, 185,  15); }
         |  'darkgoldenrod2'i       %{ return RGB256(238, 173,  14); }
         |  'darkgoldenrod3'i       %{ return RGB256(205, 149,  12); }
         |  'darkgoldenrod4'i       %{ return RGB256(139, 101,   8); }
         |  'darkgoldenrod'i        %{ return RGB256(184, 134,  11); }
         |  'darkgray'i             %{ return RGB256(169, 169, 169); }
         |  'darkgreen'i            %{ return RGB256(  0, 100,   0); }
         |  'darkgrey'i             %{ return RGB256(169, 169, 169); }
         |  'darkkhaki'i            %{ return RGB256(189, 183, 107); }
         |  'darkmagenta'i          %{ return RGB256(139,   0, 139); }
         |  'darkolivegreen1'i      %{ return RGB256(202, 255, 112); }
         |  'darkolivegreen2'i      %{ return RGB256(188, 238, 104); }
         |  'darkolivegreen3'i      %{ return RGB256(162, 205,  90); }
         |  'darkolivegreen4'i      %{ return RGB256(110, 139,  61); }
         |  'darkolivegreen'i       %{ return RGB256( 85, 107,  47); }
         |  'darkorange1'i          %{ return RGB256(255, 127,   0); }
         |  'darkorange2'i          %{ return RGB256(238, 118,   0); }
         |  'darkorange3'i          %{ return RGB256(205, 102,   0); }
         |  'darkorange4'i          %{ return RGB256(139,  69,   0); }
         |  'darkorange'i           %{ return RGB256(255, 140,   0); }
         |  'darkorchid1'i          %{ return RGB256(191,  62, 255); }
         |  'darkorchid2'i          %{ return RGB256(178,  58, 238); }
         |  'darkorchid3'i          %{ return RGB256(154,  50, 205); }
         |  'darkorchid4'i          %{ return RGB256(104,  34, 139); }
         |  'darkorchid'i           %{ return RGB256(153,  50, 204); }
         |  'darkred'i              %{ return RGB256(139,   0,   0); }
         |  'darksalmon'i           %{ return RGB256(233, 150, 122); }
         |  'darkseagreen1'i        %{ return RGB256(193, 255, 193); }
         |  'darkseagreen2'i        %{ return RGB256(180, 238, 180); }
         |  'darkseagreen3'i        %{ return RGB256(155, 205, 155); }
         |  'darkseagreen4'i        %{ return RGB256(105, 139, 105); }
         |  'darkseagreen'i         %{ return RGB256(143, 188, 143); }
         |  'darkslateblue'i        %{ return RGB256( 72,  61, 139); }
         |  'darkslategray1'i       %{ return RGB256(151, 255, 255); }
         |  'darkslategray2'i       %{ return RGB256(141, 238, 238); }
         |  'darkslategray3'i       %{ return RGB256(121, 205, 205); }
         |  'darkslategray4'i       %{ return RGB256( 82, 139, 139); }
         |  'darkslategray'i        %{ return RGB256( 47,  79,  79); }
         |  'darkslategrey'i        %{ return RGB256( 47,  79,  79); }
         |  'darkturquoise'i        %{ return RGB256(  0, 206, 209); }
         |  'darkviolet'i           %{ return RGB256(148,   0, 211); }
         |  'deeppink1'i            %{ return RGB256(255,  20, 147); }
         |  'deeppink2'i            %{ return RGB256(238,  18, 137); }
         |  'deeppink3'i            %{ return RGB256(205,  16, 118); }
         |  'deeppink4'i            %{ return RGB256(139,  10,  80); }
         |  'deeppink'i             %{ return RGB256(255,  20, 147); }
         |  'deepskyblue1'i         %{ return RGB256(  0, 191, 255); }
         |  'deepskyblue2'i         %{ return RGB256(  0, 178, 238); }
         |  'deepskyblue3'i         %{ return RGB256(  0, 154, 205); }
         |  'deepskyblue4'i         %{ return RGB256(  0, 104, 139); }
         |  'deepskyblue'i          %{ return RGB256(  0, 191, 255); }
         |  'dimgray'i              %{ return RGB256(105, 105, 105); }
         |  'dimgrey'i              %{ return RGB256(105, 105, 105); }
         |  'dodgerblue1'i          %{ return RGB256( 30, 144, 255); }
         |  'dodgerblue2'i          %{ return RGB256( 28, 134, 238); }
         |  'dodgerblue3'i          %{ return RGB256( 24, 116, 205); }
         |  'dodgerblue4'i          %{ return RGB256( 16,  78, 139); }
         |  'dodgerblue'i           %{ return RGB256( 30, 144, 255); }
         |  'firebrick1'i           %{ return RGB256(255,  48,  48); }
         |  'firebrick2'i           %{ return RGB256(238,  44,  44); }
         |  'firebrick3'i           %{ return RGB256(205,  38,  38); }
         |  'firebrick4'i           %{ return RGB256(139,  26,  26); }
         |  'firebrick'i            %{ return RGB256(178,  34,  34); }
         |  'floralwhite'i          %{ return RGB256(255, 250, 240); }
         |  'forestgreen'i          %{ return RGB256( 34, 139,  34); }
         |  'gainsboro'i            %{ return RGB256(220, 220, 220); }
         |  'ghostwhite'i           %{ return RGB256(248, 248, 255); }
         |  'gold1'i                %{ return RGB256(255, 215,   0); }
         |  'gold2'i                %{ return RGB256(238, 201,   0); }
         |  'gold3'i                %{ return RGB256(205, 173,   0); }
         |  'gold4'i                %{ return RGB256(139, 117,   0); }
         |  'goldenrod1'i           %{ return RGB256(255, 193,  37); }
         |  'goldenrod2'i           %{ return RGB256(238, 180,  34); }
         |  'goldenrod3'i           %{ return RGB256(205, 155,  29); }
         |  'goldenrod4'i           %{ return RGB256(139, 105,  20); }
         |  'goldenrod'i            %{ return RGB256(218, 165,  32); }
         |  'gold'i                 %{ return RGB256(255, 215,   0); }
         |  'gray0'i                %{ return RGB256(  0,   0,   0); }
         |  'gray100'i              %{ return RGB256(255, 255, 255); }
         |  'gray10'i               %{ return RGB256( 26,  26,  26); }
         |  'gray11'i               %{ return RGB256( 28,  28,  28); }
         |  'gray12'i               %{ return RGB256( 31,  31,  31); }
         |  'gray13'i               %{ return RGB256( 33,  33,  33); }
         |  'gray14'i               %{ return RGB256( 36,  36,  36); }
         |  'gray15'i               %{ return RGB256( 38,  38,  38); }
         |  'gray16'i               %{ return RGB256( 41,  41,  41); }
         |  'gray17'i               %{ return RGB256( 43,  43,  43); }
         |  'gray18'i               %{ return RGB256( 46,  46,  46); }
         |  'gray19'i               %{ return RGB256( 48,  48,  48); }
         |  'gray1'i                %{ return RGB256(  3,   3,   3); }
         |  'gray20'i               %{ return RGB256( 51,  51,  51); }
         |  'gray21'i               %{ return RGB256( 54,  54,  54); }
         |  'gray22'i               %{ return RGB256( 56,  56,  56); }
         |  'gray23'i               %{ return RGB256( 59,  59,  59); }
         |  'gray24'i               %{ return RGB256( 61,  61,  61); }
         |  'gray25'i               %{ return RGB256( 64,  64,  64); }
         |  'gray26'i               %{ return RGB256( 66,  66,  66); }
         |  'gray27'i               %{ return RGB256( 69,  69,  69); }
         |  'gray28'i               %{ return RGB256( 71,  71,  71); }
         |  'gray29'i               %{ return RGB256( 74,  74,  74); }
         |  'gray2'i                %{ return RGB256(  5,   5,   5); }
         |  'gray30'i               %{ return RGB256( 77,  77,  77); }
         |  'gray31'i               %{ return RGB256( 79,  79,  79); }
         |  'gray32'i               %{ return RGB256( 82,  82,  82); }
         |  'gray33'i               %{ return RGB256( 84,  84,  84); }
         |  'gray34'i               %{ return RGB256( 87,  87,  87); }
         |  'gray35'i               %{ return RGB256( 89,  89,  89); }
         |  'gray36'i               %{ return RGB256( 92,  92,  92); }
         |  'gray37'i               %{ return RGB256( 94,  94,  94); }
         |  'gray38'i               %{ return RGB256( 97,  97,  97); }
         |  'gray39'i               %{ return RGB256( 99,  99,  99); }
         |  'gray3'i                %{ return RGB256(  8,   8,   8); }
         |  'gray40'i               %{ return RGB256(102, 102, 102); }
         |  'gray41'i               %{ return RGB256(105, 105, 105); }
         |  'gray42'i               %{ return RGB256(107, 107, 107); }
         |  'gray43'i               %{ return RGB256(110, 110, 110); }
         |  'gray44'i               %{ return RGB256(112, 112, 112); }
         |  'gray45'i               %{ return RGB256(115, 115, 115); }
         |  'gray46'i               %{ return RGB256(117, 117, 117); }
         |  'gray47'i               %{ return RGB256(120, 120, 120); }
         |  'gray48'i               %{ return RGB256(122, 122, 122); }
         |  'gray49'i               %{ return RGB256(125, 125, 125); }
         |  'gray4'i                %{ return RGB256( 10,  10,  10); }
         |  'gray50'i               %{ return RGB256(127, 127, 127); }
         |  'gray51'i               %{ return RGB256(130, 130, 130); }
         |  'gray52'i               %{ return RGB256(133, 133, 133); }
         |  'gray53'i               %{ return RGB256(135, 135, 135); }
         |  'gray54'i               %{ return RGB256(138, 138, 138); }
         |  'gray55'i               %{ return RGB256(140, 140, 140); }
         |  'gray56'i               %{ return RGB256(143, 143, 143); }
         |  'gray57'i               %{ return RGB256(145, 145, 145); }
         |  'gray58'i               %{ return RGB256(148, 148, 148); }
         |  'gray59'i               %{ return RGB256(150, 150, 150); }
         |  'gray5'i                %{ return RGB256( 13,  13,  13); }
         |  'gray60'i               %{ return RGB256(153, 153, 153); }
         |  'gray61'i               %{ return RGB256(156, 156, 156); }
         |  'gray62'i               %{ return RGB256(158, 158, 158); }
         |  'gray63'i               %{ return RGB256(161, 161, 161); }
         |  'gray64'i               %{ return RGB256(163, 163, 163); }
         |  'gray65'i               %{ return RGB256(166, 166, 166); }
         |  'gray66'i               %{ return RGB256(168, 168, 168); }
         |  'gray67'i               %{ return RGB256(171, 171, 171); }
         |  'gray68'i               %{ return RGB256(173, 173, 173); }
         |  'gray69'i               %{ return RGB256(176, 176, 176); }
         |  'gray6'i                %{ return RGB256( 15,  15,  15); }
         |  'gray70'i               %{ return RGB256(179, 179, 179); }
         |  'gray71'i               %{ return RGB256(181, 181, 181); }
         |  'gray72'i               %{ return RGB256(184, 184, 184); }
         |  'gray73'i               %{ return RGB256(186, 186, 186); }
         |  'gray74'i               %{ return RGB256(189, 189, 189); }
         |  'gray75'i               %{ return RGB256(191, 191, 191); }
         |  'gray76'i               %{ return RGB256(194, 194, 194); }
         |  'gray77'i               %{ return RGB256(196, 196, 196); }
         |  'gray78'i               %{ return RGB256(199, 199, 199); }
         |  'gray79'i               %{ return RGB256(201, 201, 201); }
         |  'gray7'i                %{ return RGB256( 18,  18,  18); }
         |  'gray80'i               %{ return RGB256(204, 204, 204); }
         |  'gray81'i               %{ return RGB256(207, 207, 207); }
         |  'gray82'i               %{ return RGB256(209, 209, 209); }
         |  'gray83'i               %{ return RGB256(212, 212, 212); }
         |  'gray84'i               %{ return RGB256(214, 214, 214); }
         |  'gray85'i               %{ return RGB256(217, 217, 217); }
         |  'gray86'i               %{ return RGB256(219, 219, 219); }
         |  'gray87'i               %{ return RGB256(222, 222, 222); }
         |  'gray88'i               %{ return RGB256(224, 224, 224); }
         |  'gray89'i               %{ return RGB256(227, 227, 227); }
         |  'gray8'i                %{ return RGB256( 20,  20,  20); }
         |  'gray90'i               %{ return RGB256(229, 229, 229); }
         |  'gray91'i               %{ return RGB256(232, 232, 232); }
         |  'gray92'i               %{ return RGB256(235, 235, 235); }
         |  'gray93'i               %{ return RGB256(237, 237, 237); }
         |  'gray94'i               %{ return RGB256(240, 240, 240); }
         |  'gray95'i               %{ return RGB256(242, 242, 242); }
         |  'gray96'i               %{ return RGB256(245, 245, 245); }
         |  'gray97'i               %{ return RGB256(247, 247, 247); }
         |  'gray98'i               %{ return RGB256(250, 250, 250); }
         |  'gray99'i               %{ return RGB256(252, 252, 252); }
         |  'gray9'i                %{ return RGB256( 23,  23,  23); }
         |  'gray'i                 %{ return RGB256(190, 190, 190); }
         |  'green1'i               %{ return RGB256(  0, 255,   0); }
         |  'green2'i               %{ return RGB256(  0, 238,   0); }
         |  'green3'i               %{ return RGB256(  0, 205,   0); }
         |  'green4'i               %{ return RGB256(  0, 139,   0); }
         |  'green'i                %{ return RGB256(  0, 255,   0); }
         |  'greenyellow'i          %{ return RGB256(173, 255,  47); }
         |  'grey0'i                %{ return RGB256(  0,   0,   0); }
         |  'grey100'i              %{ return RGB256(255, 255, 255); }
         |  'grey10'i               %{ return RGB256( 26,  26,  26); }
         |  'grey11'i               %{ return RGB256( 28,  28,  28); }
         |  'grey12'i               %{ return RGB256( 31,  31,  31); }
         |  'grey13'i               %{ return RGB256( 33,  33,  33); }
         |  'grey14'i               %{ return RGB256( 36,  36,  36); }
         |  'grey15'i               %{ return RGB256( 38,  38,  38); }
         |  'grey16'i               %{ return RGB256( 41,  41,  41); }
         |  'grey17'i               %{ return RGB256( 43,  43,  43); }
         |  'grey18'i               %{ return RGB256( 46,  46,  46); }
         |  'grey19'i               %{ return RGB256( 48,  48,  48); }
         |  'grey1'i                %{ return RGB256(  3,   3,   3); }
         |  'grey20'i               %{ return RGB256( 51,  51,  51); }
         |  'grey21'i               %{ return RGB256( 54,  54,  54); }
         |  'grey22'i               %{ return RGB256( 56,  56,  56); }
         |  'grey23'i               %{ return RGB256( 59,  59,  59); }
         |  'grey24'i               %{ return RGB256( 61,  61,  61); }
         |  'grey25'i               %{ return RGB256( 64,  64,  64); }
         |  'grey26'i               %{ return RGB256( 66,  66,  66); }
         |  'grey27'i               %{ return RGB256( 69,  69,  69); }
         |  'grey28'i               %{ return RGB256( 71,  71,  71); }
         |  'grey29'i               %{ return RGB256( 74,  74,  74); }
         |  'grey2'i                %{ return RGB256(  5,   5,   5); }
         |  'grey30'i               %{ return RGB256( 77,  77,  77); }
         |  'grey31'i               %{ return RGB256( 79,  79,  79); }
         |  'grey32'i               %{ return RGB256( 82,  82,  82); }
         |  'grey33'i               %{ return RGB256( 84,  84,  84); }
         |  'grey34'i               %{ return RGB256( 87,  87,  87); }
         |  'grey35'i               %{ return RGB256( 89,  89,  89); }
         |  'grey36'i               %{ return RGB256( 92,  92,  92); }
         |  'grey37'i               %{ return RGB256( 94,  94,  94); }
         |  'grey38'i               %{ return RGB256( 97,  97,  97); }
         |  'grey39'i               %{ return RGB256( 99,  99,  99); }
         |  'grey3'i                %{ return RGB256(  8,   8,   8); }
         |  'grey40'i               %{ return RGB256(102, 102, 102); }
         |  'grey41'i               %{ return RGB256(105, 105, 105); }
         |  'grey42'i               %{ return RGB256(107, 107, 107); }
         |  'grey43'i               %{ return RGB256(110, 110, 110); }
         |  'grey44'i               %{ return RGB256(112, 112, 112); }
         |  'grey45'i               %{ return RGB256(115, 115, 115); }
         |  'grey46'i               %{ return RGB256(117, 117, 117); }
         |  'grey47'i               %{ return RGB256(120, 120, 120); }
         |  'grey48'i               %{ return RGB256(122, 122, 122); }
         |  'grey49'i               %{ return RGB256(125, 125, 125); }
         |  'grey4'i                %{ return RGB256( 10,  10,  10); }
         |  'grey50'i               %{ return RGB256(127, 127, 127); }
         |  'grey51'i               %{ return RGB256(130, 130, 130); }
         |  'grey52'i               %{ return RGB256(133, 133, 133); }
         |  'grey53'i               %{ return RGB256(135, 135, 135); }
         |  'grey54'i               %{ return RGB256(138, 138, 138); }
         |  'grey55'i               %{ return RGB256(140, 140, 140); }
         |  'grey56'i               %{ return RGB256(143, 143, 143); }
         |  'grey57'i               %{ return RGB256(145, 145, 145); }
         |  'grey58'i               %{ return RGB256(148, 148, 148); }
         |  'grey59'i               %{ return RGB256(150, 150, 150); }
         |  'grey5'i                %{ return RGB256( 13,  13,  13); }
         |  'grey60'i               %{ return RGB256(153, 153, 153); }
         |  'grey61'i               %{ return RGB256(156, 156, 156); }
         |  'grey62'i               %{ return RGB256(158, 158, 158); }
         |  'grey63'i               %{ return RGB256(161, 161, 161); }
         |  'grey64'i               %{ return RGB256(163, 163, 163); }
         |  'grey65'i               %{ return RGB256(166, 166, 166); }
         |  'grey66'i               %{ return RGB256(168, 168, 168); }
         |  'grey67'i               %{ return RGB256(171, 171, 171); }
         |  'grey68'i               %{ return RGB256(173, 173, 173); }
         |  'grey69'i               %{ return RGB256(176, 176, 176); }
         |  'grey6'i                %{ return RGB256( 15,  15,  15); }
         |  'grey70'i               %{ return RGB256(179, 179, 179); }
         |  'grey71'i               %{ return RGB256(181, 181, 181); }
         |  'grey72'i               %{ return RGB256(184, 184, 184); }
         |  'grey73'i               %{ return RGB256(186, 186, 186); }
         |  'grey74'i               %{ return RGB256(189, 189, 189); }
         |  'grey75'i               %{ return RGB256(191, 191, 191); }
         |  'grey76'i               %{ return RGB256(194, 194, 194); }
         |  'grey77'i               %{ return RGB256(196, 196, 196); }
         |  'grey78'i               %{ return RGB256(199, 199, 199); }
         |  'grey79'i               %{ return RGB256(201, 201, 201); }
         |  'grey7'i                %{ return RGB256( 18,  18,  18); }
         |  'grey80'i               %{ return RGB256(204, 204, 204); }
         |  'grey81'i               %{ return RGB256(207, 207, 207); }
         |  'grey82'i               %{ return RGB256(209, 209, 209); }
         |  'grey83'i               %{ return RGB256(212, 212, 212); }
         |  'grey84'i               %{ return RGB256(214, 214, 214); }
         |  'grey85'i               %{ return RGB256(217, 217, 217); }
         |  'grey86'i               %{ return RGB256(219, 219, 219); }
         |  'grey87'i               %{ return RGB256(222, 222, 222); }
         |  'grey88'i               %{ return RGB256(224, 224, 224); }
         |  'grey89'i               %{ return RGB256(227, 227, 227); }
         |  'grey8'i                %{ return RGB256( 20,  20,  20); }
         |  'grey90'i               %{ return RGB256(229, 229, 229); }
         |  'grey91'i               %{ return RGB256(232, 232, 232); }
         |  'grey92'i               %{ return RGB256(235, 235, 235); }
         |  'grey93'i               %{ return RGB256(237, 237, 237); }
         |  'grey94'i               %{ return RGB256(240, 240, 240); }
         |  'grey95'i               %{ return RGB256(242, 242, 242); }
         |  'grey96'i               %{ return RGB256(245, 245, 245); }
         |  'grey97'i               %{ return RGB256(247, 247, 247); }
         |  'grey98'i               %{ return RGB256(250, 250, 250); }
         |  'grey99'i               %{ return RGB256(252, 252, 252); }
         |  'grey9'i                %{ return RGB256( 23,  23,  23); }
         |  'grey'i                 %{ return RGB256(190, 190, 190); }
         |  'honeydew1'i            %{ return RGB256(240, 255, 240); }
         |  'honeydew2'i            %{ return RGB256(224, 238, 224); }
         |  'honeydew3'i            %{ return RGB256(193, 205, 193); }
         |  'honeydew4'i            %{ return RGB256(131, 139, 131); }
         |  'honeydew'i             %{ return RGB256(240, 255, 240); }
         |  'hotpink1'i             %{ return RGB256(255, 110, 180); }
         |  'hotpink2'i             %{ return RGB256(238, 106, 167); }
         |  'hotpink3'i             %{ return RGB256(205,  96, 144); }
         |  'hotpink4'i             %{ return RGB256(139,  58,  98); }
         |  'hotpink'i              %{ return RGB256(255, 105, 180); }
         |  'indianred1'i           %{ return RGB256(255, 106, 106); }
         |  'indianred2'i           %{ return RGB256(238,  99,  99); }
         |  'indianred3'i           %{ return RGB256(205,  85,  85); }
         |  'indianred4'i           %{ return RGB256(139,  58,  58); }
         |  'indianred'i            %{ return RGB256(205,  92,  92); }
         |  'ivory1'i               %{ return RGB256(255, 255, 240); }
         |  'ivory2'i               %{ return RGB256(238, 238, 224); }
         |  'ivory3'i               %{ return RGB256(205, 205, 193); }
         |  'ivory4'i               %{ return RGB256(139, 139, 131); }
         |  'ivory'i                %{ return RGB256(255, 255, 240); }
         |  'khaki1'i               %{ return RGB256(255, 246, 143); }
         |  'khaki2'i               %{ return RGB256(238, 230, 133); }
         |  'khaki3'i               %{ return RGB256(205, 198, 115); }
         |  'khaki4'i               %{ return RGB256(139, 134,  78); }
         |  'khaki'i                %{ return RGB256(240, 230, 140); }
         |  'lavenderblush1'i       %{ return RGB256(255, 240, 245); }
         |  'lavenderblush2'i       %{ return RGB256(238, 224, 229); }
         |  'lavenderblush3'i       %{ return RGB256(205, 193, 197); }
         |  'lavenderblush4'i       %{ return RGB256(139, 131, 134); }
         |  'lavenderblush'i        %{ return RGB256(255, 240, 245); }
         |  'lavender'i             %{ return RGB256(230, 230, 250); }
         |  'lawngreen'i            %{ return RGB256(124, 252,   0); }
         |  'lemonchiffon1'i        %{ return RGB256(255, 250, 205); }
         |  'lemonchiffon2'i        %{ return RGB256(238, 233, 191); }
         |  'lemonchiffon3'i        %{ return RGB256(205, 201, 165); }
         |  'lemonchiffon4'i        %{ return RGB256(139, 137, 112); }
         |  'lemonchiffon'i         %{ return RGB256(255, 250, 205); }
         |  'lightblue1'i           %{ return RGB256(191, 239, 255); }
         |  'lightblue2'i           %{ return RGB256(178, 223, 238); }
         |  'lightblue3'i           %{ return RGB256(154, 192, 205); }
         |  'lightblue4'i           %{ return RGB256(104, 131, 139); }
         |  'lightblue'i            %{ return RGB256(173, 216, 230); }
         |  'lightcoral'i           %{ return RGB256(240, 128, 128); }
         |  'lightcyan1'i           %{ return RGB256(224, 255, 255); }
         |  'lightcyan2'i           %{ return RGB256(209, 238, 238); }
         |  'lightcyan3'i           %{ return RGB256(180, 205, 205); }
         |  'lightcyan4'i           %{ return RGB256(122, 139, 139); }
         |  'lightcyan'i            %{ return RGB256(224, 255, 255); }
         |  'lightgoldenrod1'i      %{ return RGB256(255, 236, 139); }
         |  'lightgoldenrod2'i      %{ return RGB256(238, 220, 130); }
         |  'lightgoldenrod3'i      %{ return RGB256(205, 190, 112); }
         |  'lightgoldenrod4'i      %{ return RGB256(139, 129,  76); }
         |  'lightgoldenrod'i       %{ return RGB256(238, 221, 130); }
         |  'lightgoldenrodyellow'i %{ return RGB256(250, 250, 210); }
         |  'lightgray'i            %{ return RGB256(211, 211, 211); }
         |  'lightgreen'i           %{ return RGB256(144, 238, 144); }
         |  'lightgrey'i            %{ return RGB256(211, 211, 211); }
         |  'lightpink1'i           %{ return RGB256(255, 174, 185); }
         |  'lightpink2'i           %{ return RGB256(238, 162, 173); }
         |  'lightpink3'i           %{ return RGB256(205, 140, 149); }
         |  'lightpink4'i           %{ return RGB256(139,  95, 101); }
         |  'lightpink'i            %{ return RGB256(255, 182, 193); }
         |  'lightsalmon1'i         %{ return RGB256(255, 160, 122); }
         |  'lightsalmon2'i         %{ return RGB256(238, 149, 114); }
         |  'lightsalmon3'i         %{ return RGB256(205, 129,  98); }
         |  'lightsalmon4'i         %{ return RGB256(139,  87,  66); }
         |  'lightsalmon'i          %{ return RGB256(255, 160, 122); }
         |  'lightseagreen'i        %{ return RGB256( 32, 178, 170); }
         |  'lightskyblue1'i        %{ return RGB256(176, 226, 255); }
         |  'lightskyblue2'i        %{ return RGB256(164, 211, 238); }
         |  'lightskyblue3'i        %{ return RGB256(141, 182, 205); }
         |  'lightskyblue4'i        %{ return RGB256( 96, 123, 139); }
         |  'lightskyblue'i         %{ return RGB256(135, 206, 250); }
         |  'lightslateblue'i       %{ return RGB256(132, 112, 255); }
         |  'lightslategray'i       %{ return RGB256(119, 136, 153); }
         |  'lightslategrey'i       %{ return RGB256(119, 136, 153); }
         |  'lightsteelblue1'i      %{ return RGB256(202, 225, 255); }
         |  'lightsteelblue2'i      %{ return RGB256(188, 210, 238); }
         |  'lightsteelblue3'i      %{ return RGB256(162, 181, 205); }
         |  'lightsteelblue4'i      %{ return RGB256(110, 123, 139); }
         |  'lightsteelblue'i       %{ return RGB256(176, 196, 222); }
         |  'lightyellow1'i         %{ return RGB256(255, 255, 224); }
         |  'lightyellow2'i         %{ return RGB256(238, 238, 209); }
         |  'lightyellow3'i         %{ return RGB256(205, 205, 180); }
         |  'lightyellow4'i         %{ return RGB256(139, 139, 122); }
         |  'lightyellow'i          %{ return RGB256(255, 255, 224); }
         |  'limegreen'i            %{ return RGB256( 50, 205,  50); }
         |  'linen'i                %{ return RGB256(250, 240, 230); }
         |  'magenta1'i             %{ return RGB256(255,   0, 255); }
         |  'magenta2'i             %{ return RGB256(238,   0, 238); }
         |  'magenta3'i             %{ return RGB256(205,   0, 205); }
         |  'magenta4'i             %{ return RGB256(139,   0, 139); }
         |  'magenta'i              %{ return RGB256(255,   0, 255); }
         |  'maroon1'i              %{ return RGB256(255,  52, 179); }
         |  'maroon2'i              %{ return RGB256(238,  48, 167); }
         |  'maroon3'i              %{ return RGB256(205,  41, 144); }
         |  'maroon4'i              %{ return RGB256(139,  28,  98); }
         |  'maroon'i               %{ return RGB256(176,  48,  96); }
         |  'mediumaquamarine'i     %{ return RGB256(102, 205, 170); }
         |  'mediumblue'i           %{ return RGB256(  0,   0, 205); }
         |  'mediumorchid1'i        %{ return RGB256(224, 102, 255); }
         |  'mediumorchid2'i        %{ return RGB256(209,  95, 238); }
         |  'mediumorchid3'i        %{ return RGB256(180,  82, 205); }
         |  'mediumorchid4'i        %{ return RGB256(122,  55, 139); }
         |  'mediumorchid'i         %{ return RGB256(186,  85, 211); }
         |  'mediumpurple1'i        %{ return RGB256(171, 130, 255); }
         |  'mediumpurple2'i        %{ return RGB256(159, 121, 238); }
         |  'mediumpurple3'i        %{ return RGB256(137, 104, 205); }
         |  'mediumpurple4'i        %{ return RGB256( 93,  71, 139); }
         |  'mediumpurple'i         %{ return RGB256(147, 112, 219); }
         |  'mediumseagreen'i       %{ return RGB256( 60, 179, 113); }
         |  'mediumslateblue'i      %{ return RGB256(123, 104, 238); }
         |  'mediumspringgreen'i    %{ return RGB256(  0, 250, 154); }
         |  'mediumturquoise'i      %{ return RGB256( 72, 209, 204); }
         |  'mediumvioletred'i      %{ return RGB256(199,  21, 133); }
         |  'midnightblue'i         %{ return RGB256( 25,  25, 112); }
         |  'mintcream'i            %{ return RGB256(245, 255, 250); }
         |  'mistyrose1'i           %{ return RGB256(255, 228, 225); }
         |  'mistyrose2'i           %{ return RGB256(238, 213, 210); }
         |  'mistyrose3'i           %{ return RGB256(205, 183, 181); }
         |  'mistyrose4'i           %{ return RGB256(139, 125, 123); }
         |  'mistyrose'i            %{ return RGB256(255, 228, 225); }
         |  'moccasin'i             %{ return RGB256(255, 228, 181); }
         |  'navajowhite1'i         %{ return RGB256(255, 222, 173); }
         |  'navajowhite2'i         %{ return RGB256(238, 207, 161); }
         |  'navajowhite3'i         %{ return RGB256(205, 179, 139); }
         |  'navajowhite4'i         %{ return RGB256(139, 121,  94); }
         |  'navajowhite'i          %{ return RGB256(255, 222, 173); }
         |  'navyblue'i             %{ return RGB256(  0,   0, 128); }
         |  'navy'i                 %{ return RGB256(  0,   0, 128); }
         |  'oldlace'i              %{ return RGB256(253, 245, 230); }
         |  'olivedrab1'i           %{ return RGB256(192, 255,  62); }
         |  'olivedrab2'i           %{ return RGB256(179, 238,  58); }
         |  'olivedrab3'i           %{ return RGB256(154, 205,  50); }
         |  'olivedrab4'i           %{ return RGB256(105, 139,  34); }
         |  'olivedrab'i            %{ return RGB256(107, 142,  35); }
         |  'orange1'i              %{ return RGB256(255, 165,   0); }
         |  'orange2'i              %{ return RGB256(238, 154,   0); }
         |  'orange3'i              %{ return RGB256(205, 133,   0); }
         |  'orange4'i              %{ return RGB256(139,  90,   0); }
         |  'orange'i               %{ return RGB256(255, 165,   0); }
         |  'orangered1'i           %{ return RGB256(255,  69,   0); }
         |  'orangered2'i           %{ return RGB256(238,  64,   0); }
         |  'orangered3'i           %{ return RGB256(205,  55,   0); }
         |  'orangered4'i           %{ return RGB256(139,  37,   0); }
         |  'orangered'i            %{ return RGB256(255,  69,   0); }
         |  'orchid1'i              %{ return RGB256(255, 131, 250); }
         |  'orchid2'i              %{ return RGB256(238, 122, 233); }
         |  'orchid3'i              %{ return RGB256(205, 105, 201); }
         |  'orchid4'i              %{ return RGB256(139,  71, 137); }
         |  'orchid'i               %{ return RGB256(218, 112, 214); }
         |  'palegoldenrod'i        %{ return RGB256(238, 232, 170); }
         |  'palegreen1'i           %{ return RGB256(154, 255, 154); }
         |  'palegreen2'i           %{ return RGB256(144, 238, 144); }
         |  'palegreen3'i           %{ return RGB256(124, 205, 124); }
         |  'palegreen4'i           %{ return RGB256( 84, 139,  84); }
         |  'palegreen'i            %{ return RGB256(152, 251, 152); }
         |  'paleturquoise1'i       %{ return RGB256(187, 255, 255); }
         |  'paleturquoise2'i       %{ return RGB256(174, 238, 238); }
         |  'paleturquoise3'i       %{ return RGB256(150, 205, 205); }
         |  'paleturquoise4'i       %{ return RGB256(102, 139, 139); }
         |  'paleturquoise'i        %{ return RGB256(175, 238, 238); }
         |  'palevioletred1'i       %{ return RGB256(255, 130, 171); }
         |  'palevioletred2'i       %{ return RGB256(238, 121, 159); }
         |  'palevioletred3'i       %{ return RGB256(205, 104, 137); }
         |  'palevioletred4'i       %{ return RGB256(139,  71,  93); }
         |  'palevioletred'i        %{ return RGB256(219, 112, 147); }
         |  'papayawhip'i           %{ return RGB256(255, 239, 213); }
         |  'peachpuff1'i           %{ return RGB256(255, 218, 185); }
         |  'peachpuff2'i           %{ return RGB256(238, 203, 173); }
         |  'peachpuff3'i           %{ return RGB256(205, 175, 149); }
         |  'peachpuff4'i           %{ return RGB256(139, 119, 101); }
         |  'peachpuff'i            %{ return RGB256(255, 218, 185); }
         |  'peru'i                 %{ return RGB256(205, 133,  63); }
         |  'pink1'i                %{ return RGB256(255, 181, 197); }
         |  'pink2'i                %{ return RGB256(238, 169, 184); }
         |  'pink3'i                %{ return RGB256(205, 145, 158); }
         |  'pink4'i                %{ return RGB256(139,  99, 108); }
         |  'pink'i                 %{ return RGB256(255, 192, 203); }
         |  'plum1'i                %{ return RGB256(255, 187, 255); }
         |  'plum2'i                %{ return RGB256(238, 174, 238); }
         |  'plum3'i                %{ return RGB256(205, 150, 205); }
         |  'plum4'i                %{ return RGB256(139, 102, 139); }
         |  'plum'i                 %{ return RGB256(221, 160, 221); }
         |  'powderblue'i           %{ return RGB256(176, 224, 230); }
         |  'purple1'i              %{ return RGB256(155,  48, 255); }
         |  'purple2'i              %{ return RGB256(145,  44, 238); }
         |  'purple3'i              %{ return RGB256(125,  38, 205); }
         |  'purple4'i              %{ return RGB256( 85,  26, 139); }
         |  'purple'i               %{ return RGB256(160,  32, 240); }
         |  'red1'i                 %{ return RGB256(255,   0,   0); }
         |  'red2'i                 %{ return RGB256(238,   0,   0); }
         |  'red3'i                 %{ return RGB256(205,   0,   0); }
         |  'red4'i                 %{ return RGB256(139,   0,   0); }
         |  'red'i                  %{ return RGB256(255,   0,   0); }
         |  'rosybrown1'i           %{ return RGB256(255, 193, 193); }
         |  'rosybrown2'i           %{ return RGB256(238, 180, 180); }
         |  'rosybrown3'i           %{ return RGB256(205, 155, 155); }
         |  'rosybrown4'i           %{ return RGB256(139, 105, 105); }
         |  'rosybrown'i            %{ return RGB256(188, 143, 143); }
         |  'royalblue1'i           %{ return RGB256( 72, 118, 255); }
         |  'royalblue2'i           %{ return RGB256( 67, 110, 238); }
         |  'royalblue3'i           %{ return RGB256( 58,  95, 205); }
         |  'royalblue4'i           %{ return RGB256( 39,  64, 139); }
         |  'royalblue'i            %{ return RGB256( 65, 105, 225); }
         |  'saddlebrown'i          %{ return RGB256(139,  69,  19); }
         |  'salmon1'i              %{ return RGB256(255, 140, 105); }
         |  'salmon2'i              %{ return RGB256(238, 130,  98); }
         |  'salmon3'i              %{ return RGB256(205, 112,  84); }
         |  'salmon4'i              %{ return RGB256(139,  76,  57); }
         |  'salmon'i               %{ return RGB256(250, 128, 114); }
         |  'sandybrown'i           %{ return RGB256(244, 164,  96); }
         |  'seagreen1'i            %{ return RGB256( 84, 255, 159); }
         |  'seagreen2'i            %{ return RGB256( 78, 238, 148); }
         |  'seagreen3'i            %{ return RGB256( 67, 205, 128); }
         |  'seagreen4'i            %{ return RGB256( 46, 139,  87); }
         |  'seagreen'i             %{ return RGB256( 46, 139,  87); }
         |  'seashell1'i            %{ return RGB256(255, 245, 238); }
         |  'seashell2'i            %{ return RGB256(238, 229, 222); }
         |  'seashell3'i            %{ return RGB256(205, 197, 191); }
         |  'seashell4'i            %{ return RGB256(139, 134, 130); }
         |  'seashell'i             %{ return RGB256(255, 245, 238); }
         |  'sienna1'i              %{ return RGB256(255, 130,  71); }
         |  'sienna2'i              %{ return RGB256(238, 121,  66); }
         |  'sienna3'i              %{ return RGB256(205, 104,  57); }
         |  'sienna4'i              %{ return RGB256(139,  71,  38); }
         |  'sienna'i               %{ return RGB256(160,  82,  45); }
         |  'skyblue1'i             %{ return RGB256(135, 206, 255); }
         |  'skyblue2'i             %{ return RGB256(126, 192, 238); }
         |  'skyblue3'i             %{ return RGB256(108, 166, 205); }
         |  'skyblue4'i             %{ return RGB256( 74, 112, 139); }
         |  'skyblue'i              %{ return RGB256(135, 206, 235); }
         |  'slateblue1'i           %{ return RGB256(131, 111, 255); }
         |  'slateblue2'i           %{ return RGB256(122, 103, 238); }
         |  'slateblue3'i           %{ return RGB256(105,  89, 205); }
         |  'slateblue4'i           %{ return RGB256( 71,  60, 139); }
         |  'slateblue'i            %{ return RGB256(106,  90, 205); }
         |  'slategray1'i           %{ return RGB256(198, 226, 255); }
         |  'slategray2'i           %{ return RGB256(185, 211, 238); }
         |  'slategray3'i           %{ return RGB256(159, 182, 205); }
         |  'slategray4'i           %{ return RGB256(108, 123, 139); }
         |  'slategray'i            %{ return RGB256(112, 128, 144); }
         |  'slategrey'i            %{ return RGB256(112, 128, 144); }
         |  'snow1'i                %{ return RGB256(255, 250, 250); }
         |  'snow2'i                %{ return RGB256(238, 233, 233); }
         |  'snow3'i                %{ return RGB256(205, 201, 201); }
         |  'snow4'i                %{ return RGB256(139, 137, 137); }
         |  'snow'i                 %{ return RGB256(255, 250, 250); }
         |  'springgreen1'i         %{ return RGB256(  0, 255, 127); }
         |  'springgreen2'i         %{ return RGB256(  0, 238, 118); }
         |  'springgreen3'i         %{ return RGB256(  0, 205, 102); }
         |  'springgreen4'i         %{ return RGB256(  0, 139,  69); }
         |  'springgreen'i          %{ return RGB256(  0, 255, 127); }
         |  'steelblue1'i           %{ return RGB256( 99, 184, 255); }
         |  'steelblue2'i           %{ return RGB256( 92, 172, 238); }
         |  'steelblue3'i           %{ return RGB256( 79, 148, 205); }
         |  'steelblue4'i           %{ return RGB256( 54, 100, 139); }
         |  'steelblue'i            %{ return RGB256( 70, 130, 180); }
         |  'tan1'i                 %{ return RGB256(255, 165,  79); }
         |  'tan2'i                 %{ return RGB256(238, 154,  73); }
         |  'tan3'i                 %{ return RGB256(205, 133,  63); }
         |  'tan4'i                 %{ return RGB256(139,  90,  43); }
         |  'tan'i                  %{ return RGB256(210, 180, 140); }
         |  'thistle1'i             %{ return RGB256(255, 225, 255); }
         |  'thistle2'i             %{ return RGB256(238, 210, 238); }
         |  'thistle3'i             %{ return RGB256(205, 181, 205); }
         |  'thistle4'i             %{ return RGB256(139, 123, 139); }
         |  'thistle'i              %{ return RGB256(216, 191, 216); }
         |  'tomato1'i              %{ return RGB256(255,  99,  71); }
         |  'tomato2'i              %{ return RGB256(238,  92,  66); }
         |  'tomato3'i              %{ return RGB256(205,  79,  57); }
         |  'tomato4'i              %{ return RGB256(139,  54,  38); }
         |  'tomato'i               %{ return RGB256(255,  99,  71); }
         |  'turquoise1'i           %{ return RGB256(  0, 245, 255); }
         |  'turquoise2'i           %{ return RGB256(  0, 229, 238); }
         |  'turquoise3'i           %{ return RGB256(  0, 197, 205); }
         |  'turquoise4'i           %{ return RGB256(  0, 134, 139); }
         |  'turquoise'i            %{ return RGB256( 64, 224, 208); }
         |  'violet'i               %{ return RGB256(238, 130, 238); }
         |  'violetred1'i           %{ return RGB256(255,  62, 150); }
         |  'violetred2'i           %{ return RGB256(238,  58, 140); }
         |  'violetred3'i           %{ return RGB256(205,  50, 120); }
         |  'violetred4'i           %{ return RGB256(139,  34,  82); }
         |  'violetred'i            %{ return RGB256(208,  32, 144); }
         |  'wheat1'i               %{ return RGB256(255, 231, 186); }
         |  'wheat2'i               %{ return RGB256(238, 216, 174); }
         |  'wheat3'i               %{ return RGB256(205, 186, 150); }
         |  'wheat4'i               %{ return RGB256(139, 126, 102); }
         |  'wheat'i                %{ return RGB256(245, 222, 179); }
         |  'whitesmoke'i           %{ return RGB256(245, 245, 245); }
         |  'yellow1'i              %{ return RGB256(255, 255,   0); }
         |  'yellow2'i              %{ return RGB256(238, 238,   0); }
         |  'yellow3'i              %{ return RGB256(205, 205,   0); }
         |  'yellow4'i              %{ return RGB256(139, 139,   0); }
         |  'yellowgreen'i          %{ return RGB256(154, 205,  50); }
         |  'yellow'i               %{ return RGB256(255, 255,   0); }
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
