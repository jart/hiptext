// hiptext - Image to Text Converter
// By Justine Tunney

#include "xterm256.h"
#include <gtest/gtest.h>
#include "pixel.h"

TEST(Xterm256Test, Basic) {
  EXPECT_EQ(16, rgb_to_xterm256({0, 0, 0}));
  EXPECT_EQ(88, rgb_to_xterm256({150, 0, 0}));
  EXPECT_EQ(52, rgb_to_xterm256({77, 0, 0}));
  EXPECT_EQ(22, rgb_to_xterm256({0, 77, 0}));
  EXPECT_EQ(17, rgb_to_xterm256({0, 0, 77}));
  EXPECT_EQ(53, rgb_to_xterm256({77, 0, 77}));
  EXPECT_EQ(23, rgb_to_xterm256({0, 77, 77}));
  EXPECT_EQ(58, rgb_to_xterm256({77, 77, 0}));
  EXPECT_EQ(239, rgb_to_xterm256({77, 77, 77}));
  EXPECT_EQ(231, rgb_to_xterm256({255, 255, 255}));
  EXPECT_EQ(51, rgb_to_xterm256({0, 255, 255}));
  EXPECT_EQ(201, rgb_to_xterm256({255, 0, 255}));
  EXPECT_EQ(226, rgb_to_xterm256({255, 255, 0}));
  EXPECT_EQ(46, rgb_to_xterm256({0, 255, 0}));
  EXPECT_EQ(231, rgb_to_xterm256({255, 255, 255}));
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
