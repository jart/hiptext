// hiptext - Image to Text Converter
// By Justine Tunney

// unicode.h - Doing the standard committee's job for them.
// You need to set: std::locale::global(std::locale(getenv("LANG")))

#ifndef HIPTEXT_UNICODE_H_
#define HIPTEXT_UNICODE_H_

#include <ostream>
#include <string>

std::wstring DecodeText(const std::string& str);
std::string EncodeText(const std::wstring& wstr);
std::string EncodeText(wchar_t wch);
std::ostream& operator<<(std::ostream& os, const std::wstring& wstr);
std::ostream& operator<<(std::ostream& os, wchar_t wch);

#endif  // HIPTEXT_UNICODE_H_

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
