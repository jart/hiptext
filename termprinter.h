// hiptext - Image to Text Converter
// By Justine Tunney

#ifndef HIPTEXT_TERMPRINTER_H_
#define HIPTEXT_TERMPRINTER_H_

#include <ostream>

// A wrapper around cout that compresses term color escape codes.
class TermPrinter {
 public:
  // 'bg' is the native background color of the terminal. If 'bgprint' is set
  // to false then TermPrinter will not waste its time printing 256color
  // background codes that are nearly identical to your terminal background.
  explicit TermPrinter(std::ostream& out);

  void Flush();
  void Reset(bool force = false);
  void SetBold(bool bold);
  void SetItalic(bool italic);
  void SetUnderline(bool underline);
  void SetUnderline2(bool underline2);
  void SetStrike(bool strike);
  void SetBlink(bool blink);
  void SetFlip(bool flip);
  void SetForeground256(int code);
  void SetBackground256(int code);

  template<typename T>
  inline TermPrinter& operator<<(const T& val) {
    Flush();
    out_ << val;
    return *this;
  }

 private:
  static const int kReset;
  static const int kBoldOn;
  static const int kBoldOff;
  static const int kItalicOn;
  static const int kItalicOff;
  static const int kUnderlineOn;
  static const int kUnderlineOff;
  static const int kUnderline2On;
  static const int kUnderline2Off;
  static const int kStrikeOn;
  static const int kStrikeOff;
  static const int kBlinkOn;
  static const int kBlinkOff;
  static const int kFlipOn;
  static const int kFlipOff;
  static const int kForegroundOff;
  static const int kBackgroundOff;
  static const int kForeground256;
  static const int kBackground256;
  static const char* kEscapeStart;
  static const char* kEscapeEnd;
  static const char* kEscapeSep;
  static const char* kEscapeReset;

  struct State {
    int fg;
    int bg;
    bool bold;
    bool italic;
    bool underline;
    bool underline2;
    bool strike;
    bool blink;
    bool flip;
  };

  bool IsStyled() const;
  void PrintSep(bool* first) const;
  void PrintCode(int code, bool* first);

  bool dirty_;
  State cur_;
  State new_;
  std::ostream& out_;
};

#endif  // HIPTEXT_TERMPRINTER_H_

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
