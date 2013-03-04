// hiptext - Image to Text Converter
// Copyright (c) 2013 Justine Tunney

#include "unicode.h"
#include <cstdlib>
#include <locale>
#include <glog/logging.h>

std::wstring DecodeText(const std::string& str) {
  std::wstring res(str.size() + 1, L'\0');
  size_t count = std::mbstowcs(&res.front(), &str.front(), res.size());
  CHECK(count != (size_t)-1) << "Invalid text: " << str;
  res.resize(count);
  return res;
}

std::string EncodeText(const std::wstring& wstr) {
  std::string res(wstr.size() * 4, '\0');
  size_t count = std::wcstombs(&res.front(), &wstr.front(), res.size());
  CHECK(count != (size_t)-1) << "Invalid wstring";
  res.resize(count);
  return res;
}

std::string EncodeText(wchar_t wch) {
  std::string res(MB_CUR_MAX + 1, '\0');
  size_t count = std::wcrtomb(&res.front(), wch, NULL);
  CHECK(count != (size_t)-1) << "Invalid wchar_t: " << static_cast<int>(wch);
  res.resize(count);
  return res;
}

std::ostream& operator<<(std::ostream& os, const std::wstring& wstr) {
  os << EncodeText(wstr);
  return os;
}

std::ostream& operator<<(std::ostream& os, wchar_t wch) {
  os << EncodeText(wch);
  return os;
}

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
