# hiptext - Render Images and Videos as Text

![Last Supper in Macterm](http://i.imgur.com/7TfrQsL.png)

![Happy Cat in Macterm](http://i.imgur.com/cr7sSHh.png)

![Obama in Macterm](http://i.imgur.com/UnfnpMr.png)

Hiptext is a fun little research project of Justine Tunney to find new and
interesting ways to render images as text, with a lot of help from Serene Han.

## Dependencies

You need clang or gcc >=4.7. You also need to install:

    sudo apt-get install build-essential libpng12-dev libjpeg-dev \
        libfreetype6-dev libgif-dev ragel libavformat-dev libavcodec-dev \
        libswscale-dev libgflags-dev libgoogle-glog-dev

## Invocation

    make
    ./hiptext whatever.png
    ./hiptext whatever.avi
    ./hiptext --spectrum                        # Show color spectrum graph
    ./hiptext --macterm whatever.png            # The absolute best (Mac only)
    ./hiptext --xterm256unicode whatever.png    # Best on xterm/gnome-terminal
    ./hiptext --xterm256 whatever.png           # Default
    ./hiptext --nocolor whatever.png
    ./hiptext --nocolor --chars=" .oO0" whatever.png
