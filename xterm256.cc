#include "xterm256.h"
#include "pixel.h"
#include <glog/logging.h>

static uint8_t g_cube_steps[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

// These colors vary wildly based on the palette of the terminal emulator.
static Pixel g_basic16[] = {
  RGB256(  0,   0,   0), RGB256(205,   0,   0), RGB256(  0, 205,   0),
  RGB256(205, 205,   0), RGB256(  0,   0, 238), RGB256(205,   0, 205),
  RGB256(  0, 205, 205), RGB256(229, 229, 229), RGB256(127, 127, 127),
  RGB256(255,   0,   0), RGB256(  0, 255,   0), RGB256(255, 255,   0),
  RGB256( 92,  92, 255), RGB256(255,   0, 255), RGB256(  0, 255, 255),
  RGB256(255, 255, 255)
};

constexpr Pixel CalculateXtermToRGB(uint8_t xcolor) {
  return ((xcolor < 16)
          ? g_basic16[xcolor]
          : ((xcolor < 232)
             ? RGB256(g_cube_steps[((xcolor - 16) / 36) % 6],
                      g_cube_steps[((xcolor - 16) / 6) % 6],
                      g_cube_steps[(xcolor - 16) % 6])
             : Grey256(8 + (xcolor - 232) * 0x0A)));
}

uint8_t rgb_to_xterm(const Pixel& pix, int begin, int end) {
  DCHECK(0 <= begin && begin <= 256) << begin;
  DCHECK(0 <= end && end <= 256) << end;
  uint8_t best_match = 0;
  double smallest_distance = 1e9;
  for (int c = begin; c < end; ++c) {
    double dist = g_xterm[c].Distance(pix);
    if (dist < smallest_distance) {
      smallest_distance = dist;
      best_match = c;
    }
  }
  return best_match;
}

uint8_t rgb_to_xterm16(const Pixel& pix) {
  return rgb_to_xterm(pix, 0, 16);
}

uint8_t rgb_to_xterm256(const Pixel& pix) {
  return rgb_to_xterm(pix, 16, 256);
}

const Pixel& xterm_to_rgb(int code) {
  DCHECK(0 <= code && code < 256) << code;
  return g_xterm[code];
}

void PrintXterm256() {
  for (int n = 0; n < 256; n++) {
    Pixel pix = g_xterm[n];
    printf("\x1b[48;5;%dm", n);
    printf("%d = (%0.2f, %0.2f, %0.2f)", n, pix.red(), pix.green(), pix.blue());
    printf("\x1b[0m\n");
  }
}

Pixel g_xterm[256] = {
  CalculateXtermToRGB(0),
  CalculateXtermToRGB(1),
  CalculateXtermToRGB(2),
  CalculateXtermToRGB(3),
  CalculateXtermToRGB(4),
  CalculateXtermToRGB(5),
  CalculateXtermToRGB(6),
  CalculateXtermToRGB(7),
  CalculateXtermToRGB(8),
  CalculateXtermToRGB(9),
  CalculateXtermToRGB(10),
  CalculateXtermToRGB(11),
  CalculateXtermToRGB(12),
  CalculateXtermToRGB(13),
  CalculateXtermToRGB(14),
  CalculateXtermToRGB(15),
  CalculateXtermToRGB(16),
  CalculateXtermToRGB(17),
  CalculateXtermToRGB(18),
  CalculateXtermToRGB(19),
  CalculateXtermToRGB(20),
  CalculateXtermToRGB(21),
  CalculateXtermToRGB(22),
  CalculateXtermToRGB(23),
  CalculateXtermToRGB(24),
  CalculateXtermToRGB(25),
  CalculateXtermToRGB(26),
  CalculateXtermToRGB(27),
  CalculateXtermToRGB(28),
  CalculateXtermToRGB(29),
  CalculateXtermToRGB(30),
  CalculateXtermToRGB(31),
  CalculateXtermToRGB(32),
  CalculateXtermToRGB(33),
  CalculateXtermToRGB(34),
  CalculateXtermToRGB(35),
  CalculateXtermToRGB(36),
  CalculateXtermToRGB(37),
  CalculateXtermToRGB(38),
  CalculateXtermToRGB(39),
  CalculateXtermToRGB(40),
  CalculateXtermToRGB(41),
  CalculateXtermToRGB(42),
  CalculateXtermToRGB(43),
  CalculateXtermToRGB(44),
  CalculateXtermToRGB(45),
  CalculateXtermToRGB(46),
  CalculateXtermToRGB(47),
  CalculateXtermToRGB(48),
  CalculateXtermToRGB(49),
  CalculateXtermToRGB(50),
  CalculateXtermToRGB(51),
  CalculateXtermToRGB(52),
  CalculateXtermToRGB(53),
  CalculateXtermToRGB(54),
  CalculateXtermToRGB(55),
  CalculateXtermToRGB(56),
  CalculateXtermToRGB(57),
  CalculateXtermToRGB(58),
  CalculateXtermToRGB(59),
  CalculateXtermToRGB(60),
  CalculateXtermToRGB(61),
  CalculateXtermToRGB(62),
  CalculateXtermToRGB(63),
  CalculateXtermToRGB(64),
  CalculateXtermToRGB(65),
  CalculateXtermToRGB(66),
  CalculateXtermToRGB(67),
  CalculateXtermToRGB(68),
  CalculateXtermToRGB(69),
  CalculateXtermToRGB(70),
  CalculateXtermToRGB(71),
  CalculateXtermToRGB(72),
  CalculateXtermToRGB(73),
  CalculateXtermToRGB(74),
  CalculateXtermToRGB(75),
  CalculateXtermToRGB(76),
  CalculateXtermToRGB(77),
  CalculateXtermToRGB(78),
  CalculateXtermToRGB(79),
  CalculateXtermToRGB(80),
  CalculateXtermToRGB(81),
  CalculateXtermToRGB(82),
  CalculateXtermToRGB(83),
  CalculateXtermToRGB(84),
  CalculateXtermToRGB(85),
  CalculateXtermToRGB(86),
  CalculateXtermToRGB(87),
  CalculateXtermToRGB(88),
  CalculateXtermToRGB(89),
  CalculateXtermToRGB(90),
  CalculateXtermToRGB(91),
  CalculateXtermToRGB(92),
  CalculateXtermToRGB(93),
  CalculateXtermToRGB(94),
  CalculateXtermToRGB(95),
  CalculateXtermToRGB(96),
  CalculateXtermToRGB(97),
  CalculateXtermToRGB(98),
  CalculateXtermToRGB(99),
  CalculateXtermToRGB(100),
  CalculateXtermToRGB(101),
  CalculateXtermToRGB(102),
  CalculateXtermToRGB(103),
  CalculateXtermToRGB(104),
  CalculateXtermToRGB(105),
  CalculateXtermToRGB(106),
  CalculateXtermToRGB(107),
  CalculateXtermToRGB(108),
  CalculateXtermToRGB(109),
  CalculateXtermToRGB(110),
  CalculateXtermToRGB(111),
  CalculateXtermToRGB(112),
  CalculateXtermToRGB(113),
  CalculateXtermToRGB(114),
  CalculateXtermToRGB(115),
  CalculateXtermToRGB(116),
  CalculateXtermToRGB(117),
  CalculateXtermToRGB(118),
  CalculateXtermToRGB(119),
  CalculateXtermToRGB(120),
  CalculateXtermToRGB(121),
  CalculateXtermToRGB(122),
  CalculateXtermToRGB(123),
  CalculateXtermToRGB(124),
  CalculateXtermToRGB(125),
  CalculateXtermToRGB(126),
  CalculateXtermToRGB(127),
  CalculateXtermToRGB(128),
  CalculateXtermToRGB(129),
  CalculateXtermToRGB(130),
  CalculateXtermToRGB(131),
  CalculateXtermToRGB(132),
  CalculateXtermToRGB(133),
  CalculateXtermToRGB(134),
  CalculateXtermToRGB(135),
  CalculateXtermToRGB(136),
  CalculateXtermToRGB(137),
  CalculateXtermToRGB(138),
  CalculateXtermToRGB(139),
  CalculateXtermToRGB(140),
  CalculateXtermToRGB(141),
  CalculateXtermToRGB(142),
  CalculateXtermToRGB(143),
  CalculateXtermToRGB(144),
  CalculateXtermToRGB(145),
  CalculateXtermToRGB(146),
  CalculateXtermToRGB(147),
  CalculateXtermToRGB(148),
  CalculateXtermToRGB(149),
  CalculateXtermToRGB(150),
  CalculateXtermToRGB(151),
  CalculateXtermToRGB(152),
  CalculateXtermToRGB(153),
  CalculateXtermToRGB(154),
  CalculateXtermToRGB(155),
  CalculateXtermToRGB(156),
  CalculateXtermToRGB(157),
  CalculateXtermToRGB(158),
  CalculateXtermToRGB(159),
  CalculateXtermToRGB(160),
  CalculateXtermToRGB(161),
  CalculateXtermToRGB(162),
  CalculateXtermToRGB(163),
  CalculateXtermToRGB(164),
  CalculateXtermToRGB(165),
  CalculateXtermToRGB(166),
  CalculateXtermToRGB(167),
  CalculateXtermToRGB(168),
  CalculateXtermToRGB(169),
  CalculateXtermToRGB(170),
  CalculateXtermToRGB(171),
  CalculateXtermToRGB(172),
  CalculateXtermToRGB(173),
  CalculateXtermToRGB(174),
  CalculateXtermToRGB(175),
  CalculateXtermToRGB(176),
  CalculateXtermToRGB(177),
  CalculateXtermToRGB(178),
  CalculateXtermToRGB(179),
  CalculateXtermToRGB(180),
  CalculateXtermToRGB(181),
  CalculateXtermToRGB(182),
  CalculateXtermToRGB(183),
  CalculateXtermToRGB(184),
  CalculateXtermToRGB(185),
  CalculateXtermToRGB(186),
  CalculateXtermToRGB(187),
  CalculateXtermToRGB(188),
  CalculateXtermToRGB(189),
  CalculateXtermToRGB(190),
  CalculateXtermToRGB(191),
  CalculateXtermToRGB(192),
  CalculateXtermToRGB(193),
  CalculateXtermToRGB(194),
  CalculateXtermToRGB(195),
  CalculateXtermToRGB(196),
  CalculateXtermToRGB(197),
  CalculateXtermToRGB(198),
  CalculateXtermToRGB(199),
  CalculateXtermToRGB(200),
  CalculateXtermToRGB(201),
  CalculateXtermToRGB(202),
  CalculateXtermToRGB(203),
  CalculateXtermToRGB(204),
  CalculateXtermToRGB(205),
  CalculateXtermToRGB(206),
  CalculateXtermToRGB(207),
  CalculateXtermToRGB(208),
  CalculateXtermToRGB(209),
  CalculateXtermToRGB(210),
  CalculateXtermToRGB(211),
  CalculateXtermToRGB(212),
  CalculateXtermToRGB(213),
  CalculateXtermToRGB(214),
  CalculateXtermToRGB(215),
  CalculateXtermToRGB(216),
  CalculateXtermToRGB(217),
  CalculateXtermToRGB(218),
  CalculateXtermToRGB(219),
  CalculateXtermToRGB(220),
  CalculateXtermToRGB(221),
  CalculateXtermToRGB(222),
  CalculateXtermToRGB(223),
  CalculateXtermToRGB(224),
  CalculateXtermToRGB(225),
  CalculateXtermToRGB(226),
  CalculateXtermToRGB(227),
  CalculateXtermToRGB(228),
  CalculateXtermToRGB(229),
  CalculateXtermToRGB(230),
  CalculateXtermToRGB(231),
  CalculateXtermToRGB(232),
  CalculateXtermToRGB(233),
  CalculateXtermToRGB(234),
  CalculateXtermToRGB(235),
  CalculateXtermToRGB(236),
  CalculateXtermToRGB(237),
  CalculateXtermToRGB(238),
  CalculateXtermToRGB(239),
  CalculateXtermToRGB(240),
  CalculateXtermToRGB(241),
  CalculateXtermToRGB(242),
  CalculateXtermToRGB(243),
  CalculateXtermToRGB(244),
  CalculateXtermToRGB(245),
  CalculateXtermToRGB(246),
  CalculateXtermToRGB(247),
  CalculateXtermToRGB(248),
  CalculateXtermToRGB(249),
  CalculateXtermToRGB(250),
  CalculateXtermToRGB(251),
  CalculateXtermToRGB(252),
  CalculateXtermToRGB(253),
  CalculateXtermToRGB(254),
  CalculateXtermToRGB(255),
};

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
