
#include "hiptext/input.h"

Input::Input() {}

void Input::InitializeMain() {
  struct termios raw;

  tcgetattr(0, &backup);
  raw = backup;

  // Allow individual keys to be read(), rather than waiting for a newline char
  raw.c_lflag &= ~ICANON;
  // Prevent keypresses being echoed to the current cursor position
  raw.c_lflag &= ~ECHO;

  tcsetattr(0, TCSANOW, &raw);
}

int Input::KbHit(){
  struct timeval tv = { 0L, 0L };
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv);
}

int Input::GetCh(){
  int r;
  unsigned char c;
  if ((r = read(0, &c, sizeof(c))) < 0) {
    return r;
  } else {
    return c;
  }
}

void Input::Destructor() {
  tcsetattr(0, TCSADRAIN, &backup);
}

