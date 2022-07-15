#ifndef HIPTEXT_INPUT_H_
#define HIPTEXT_INPUT_H_

#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/termios.h>


class Input {
 public:
  explicit Input();
  Input(const Input& input);

  void InitializeMain();
  int KbHit();
  int GetCh();
  void Destructor();

 private:
  struct termios backup;
};

#endif  // HIPTEXT_INPUT_H_
