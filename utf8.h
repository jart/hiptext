// utf8.h - Tools for dealing with unicode in C++
// You need to set: std::locale::global(std::locale("en_US.utf8"))

#ifndef UTF8_H_
#define UTF8_H_

#include <ostream>
#include <string>

std::wstring DecodeUTF8(const std::string& str);
std::string EncodeUTF8(const std::wstring& wstr);
std::string EncodeUTF8(wchar_t wch);
std::ostream& operator<<(std::ostream& os, const std::wstring& wstr);
std::ostream& operator<<(std::ostream& os, wchar_t wch);

#endif  // UTF8_H_

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
