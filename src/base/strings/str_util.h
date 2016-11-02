#ifndef BASE_STRING_STR_UTIL_H_
#define BASE_STRING_STR_UTIL_H_

#include <functional>
#include <string>
#include <vector>

#include "base/gtl/array_slice.h"
#include "base/strings/strcat.h"
#include "base/stringpiece.h"
#include "base/port.h"

// Basic string utility routines
namespace base {
namespace strings {

string CEscape(const string& src);
bool CUnescape(StringPiece source, string* dest, string* error);

void StripTrailingWhitespace(string* s);

size_t RemoveLeadingWhitespace(StringPiece* text);
size_t RemoveTrailingWhitespace(StringPiece* text);
size_t RemoveWhitespaceContext(StringPiece* text);

bool ConsumeLeadingDigits(StringPiece* s, uint64* val);
bool ConsumeNonWhitespace(StringPiece* s, StringPiece* val);
bool ConsumePrefix(StringPiece* s, StringPiece expected);
bool ConsumeSuffix(StringPiece* s, StringPiece expected);

string Lowercase(StringPiece s);
string Uppercase(StringPiece s);

void TitlecaseString(string* s, StringPiece delimiters);

template <typename T>
string Join(const T& s, const char* sep);

template <typename T, typename Formatter>
string Join(const T& s, const char* sep, Formatter f);

struct AllowEmpty {
  bool operator()(StringPiece sp) const { return true; }
};

struct SkipEmpty {
  bool operator()(StringPiece sp) const { return !sp.empty(); }
};

struct SkipWhitespace {
  bool operator()(StringPiece sp) const {
    RemoveTrailingWhitespace(&sp);
    return !sp.empty();
  }
};

std::vector<string> Split(StringPiece text, char delim);
template <typename Predicate>
std::vector<string> Split(StringPiece text, char delim, Predicate p);

bool SplitAndParseAsInts(StringPiece text, char delim,
                         std::vector<int32>* result);

template <typename T>
string Join(const T& s, const char* sep) {
  string result;
  bool first = true;
  for (const auto& x : s) {
    strings::StrAppend(&result, (first ? "" : sep), x);
    first = false;
  }
  return result;
}

template <typename T>
class Formatter {
 public:
  Formatter(std::function<void(string*, T)> f) : f_(f) {}
  void operator()(string* out, const T& t) { f_(out, t); }

 private:
  std::function<void(string*, T)> f_;
};

template <typename T, typename Formatter>
string Join(const T& s, const char* sep, Formatter f) {
  string result;
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      result.append(sep);
    }
    f(&result, x);
    first = false;
  }
  return result;
}

inline std::vector<string> Split(StringPiece text, char delim) {
  return Split(text, delim, AllowEmpty());
}

template <typename Predicate>
std::vector<string> Split(StringPiece text, char delim, Predicate p) {
  std::vector<string> result;
  size_t token_start = 0;
  if (!text.empty()) {
    for (size_t i = 0; i < text.size() + 1; i++) {
      if ((i == text.size()) || (text[i] == delim)) {
        StringPiece token(text.data() + token_start, i - token_start);
        if (p(token)) {
          result.push_back(token.ToString());
        }
        token_start = i + 1;
      }
    }
  }
  return result;
}

}  // namespace strings
}  // namespace base

#endif  // BASE_STRINGS_STR_UTIL_H_
