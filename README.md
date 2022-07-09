# hiptext [![Build Status](https://travis-ci.org/jart/hiptext.svg?branch=master)](https://travis-ci.org/jart/hiptext)

hiptext is command line tool for rendering images and videos inside terminals.

![Last Supper in Macterm](http://i.imgur.com/7TfrQsL.png)

![Happy Cat in Macterm](http://i.imgur.com/cr7sSHh.png)

![Obama in Macterm](http://i.imgur.com/UnfnpMr.png)

## Dependencies

You need clang or gcc >=4.7. You also need to install:

    sudo apt-get install build-essential libpng12-dev libjpeg-dev \
        libfreetype6-dev libavformat-dev libavcodec-dev \
        libswscale-dev libgflags-dev libgoogle-glog-dev ragel

## Installation

    wget https://github.com/jart/hiptext/releases/download/0.2/hiptext-0.2.tar.gz
    tar xvzf hiptext-0.2.tar.gz
    cd hiptext-0.2

    ./configure
    make -j4
    sudo make install

If you get compile or link errors relating to freetype, and you do not have
`freetype-config(1)` on your path, try modifying `Makefile` to adjust the
`LIBFREETYPE_CFLAGS` and `LIBFREETYPE_LIBS` for your system.

## Usage

### Images

Most image types, e.g. JPEG, PNG, GIF, etc. are supported.

    hiptext balls.png

### Videos

You can play videos in your terminal using hiptext. Yes, really.

    youtube-dl -o gangnam-style.mp4 https://www.youtube.com/watch?v=9bZkp7q19f0
    hiptext gangnam-style.mp4

### Miscellaneous

    hiptext --spectrum
    hiptext --nocolor balls.png
    hiptext --nocolor --chars=" .oO0" balls.png

## Rendering Modes

### xterm256

By default, hiptext assumes you're using an xterm256 compatible terminal,
e.g. gnome-terminal, without bothering to check the `TERMINAL` environment
variable.

    hiptext --xterm256 balls.png

To double the number of pixels using Unicode half blocks, consider using the
following option, which may become the default in the future:

    hiptext --xterm256unicode balls.png

### MacTerm

The most beautiful terminal for hiptext is the one built into Mac OS X called
Terminal.app. This terminal uses xterm256 but it's implemented using a custom
color palette that was chosen by designers rather than engineers.

But the interesting thing about this palette is that it uses slightly different
colors for foreground and background. By using Unicode half blocks, this allows
us to simulate something closer to 512 colors!

    hiptext --macterm balls.png

However to use this, you *must* be using the black color scheme. After all, why
would you use anything else?

### Unicode

If you want to render an image without the ANSI color escape codes, you can use
the `--nocolor` flag. This will render the image using shaded unicode block
characters.

    hiptext --nocolor balls.png

### ASCII

The no-color mode supports a very simple character quantiser that can convert
images to ASCII. Please understand that if this is what you want, there are
much better tools for the job, like cacalib.

    hiptext --nocolor --chars=" .oO0" balls.png

### SIXEL

If you use a SIXEL terminal, e.g. mlterm >=v3.1.3, then the following flags can
provide much more minute rendering:

    hiptext --sixel256 balls.png           # For 256-color SIXEL terminal such as mlterm (>=v3.1.3)
    hiptext --sixel16 balls.png            # For 16-color SIXEL terminal such as xterm(patch level >= #294) with "-ti vt340 option"
    hiptext --sixel2 balls.png             # For monochrome SIXEL terminals

## Configuration

### Background

By default, hiptext assumes that your terminal background is black. If this is
not the case, you can specify your background color using a CSS or X11 color
string.

    hiptext --bg=white balls.png
