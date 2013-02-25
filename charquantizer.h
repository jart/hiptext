#ifndef HIPTEXT_CHARQUANTIZER_H_
#define HIPTEXT_CHARQUANTIZER_H_

#include <string>
#include <vector>

#include <glog/logging.h>

class CharQuantizer {
 public:
  CharQuantizer(const std::wstring& chars, int size) : map_(size) {
    const int segment_size = size / chars.size() + 1;
    for (int n = 0; n < size; ++n) {
      map_[n] = chars[n / segment_size];
    }
  }

  inline wchar_t Quantize(int color) const {
    DCHECK(0 <= color && color < map_.size());
    return map_[color];
  }

  CharQuantizer(const CharQuantizer& other) = delete;
  void operator=(const CharQuantizer& other) = delete;

 private:
  std::vector<wchar_t> map_;
};

#endif  // HIPTEXT_CHARQUANTIZER_H_

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
