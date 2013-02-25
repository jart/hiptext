# hiptext

## Program for rendering images as text

Dependencies:

    sudo apt-get install libpng12-dev libjpeg-dev libfreetype6-dev ttf-dejavu-core
    sudo ./install-glog.sh
    sudo ./install-gflags.sh

Demo Invocation:

    make
    ./hiptext
    ./hiptext --nocolor
    ./hiptext --nocolor --chars=" .oO0"
    ./hiptext --hinting
