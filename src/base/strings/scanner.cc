#include "base/strings/scanner.h"

namespace base {
namespace strings {

void Scanner::ScanUntilImpl(char end_ch, bool escaped) {
  for (;;) {
    if (cur_.empty()) {
      Error();
      return;
    }
    const char ch = cur_[0];
    if (ch == end_ch) {
      return;
    }

    cur_.remove_prefix(1);
    if (escaped && ch == '\\') {
      // Escape character, skip next character.
      if (cur_.empty()) {
        Error();
        return;
      }
      cur_.remove_prefix(1);
    }
  }
}

bool Scanner::GetResult(StringPiece* remaining, StringPiece* capture) {
  if (error_) {
    return false;
  }
  if (remaining != nullptr) {
    *remaining = cur_;
  }
  if (capture != nullptr) {
    const char* end = capture_end_ == nullptr ? cur_.data() : capture_end_;
    *capture = StringPiece(capture_start_, end - capture_start_);
  }
  return true;
}

}  // namespace strings
}  // namespace base
