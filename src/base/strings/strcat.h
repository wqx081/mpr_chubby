#ifndef BASE_STRINGS_STRCAT_H_
#define BASE_STRINGS_STRCAT_H_

#include <string>

#include "base/stringpiece.h"
#include "base/strings/numbers.h"
#include "base/macros.h"
#include "base/port.h"

namespace base {
namespace strings {

enum PadSpec {
  NO_PAD = 1,
  ZERO_PAD_2,
  ZERO_PAD_3,
  ZERO_PAD_4,
  ZERO_PAD_5,
  ZERO_PAD_6,
  ZERO_PAD_7,
  ZERO_PAD_8,
  ZERO_PAD_9,
  ZERO_PAD_10,
  ZERO_PAD_11,
  ZERO_PAD_12,
  ZERO_PAD_13,
  ZERO_PAD_14,
  ZERO_PAD_15,
  ZERO_PAD_16,
};

struct Hex {
  uint64 value;
  enum PadSpec spec;
  template <class Int>
  explicit Hex(Int v, PadSpec s = NO_PAD) : spec(s) {
    // Prevent sign-extension by casting integers to
    // their unsigned counterparts.
    static_assert(
        sizeof(v) == 1 || sizeof(v) == 2 || sizeof(v) == 4 || sizeof(v) == 8,
        "Unknown integer type");
    value = sizeof(v) == 1
                ? static_cast<uint8>(v)
                : sizeof(v) == 2 ? static_cast<uint16>(v)
                                 : sizeof(v) == 4 ? static_cast<uint32>(v)
                                                  : static_cast<uint64>(v);
  }
};

class AlphaNum {
 public:
  // No bool ctor -- bools convert to an integral type.
  // A bool ctor would also convert incoming pointers (bletch).

  AlphaNum(int i32)  // NOLINT(runtime/explicit)
      : piece_(digits_, FastInt32ToBufferLeft(i32, digits_) - &digits_[0]) {}
  AlphaNum(unsigned int u32)  // NOLINT(runtime/explicit)
      : piece_(digits_, FastUInt32ToBufferLeft(u32, digits_) - &digits_[0]) {}
  AlphaNum(long x)  // NOLINT(runtime/explicit)
      : piece_(digits_, FastInt64ToBufferLeft(x, digits_) - &digits_[0]) {}
  AlphaNum(unsigned long x)  // NOLINT(runtime/explicit)
      : piece_(digits_, FastUInt64ToBufferLeft(x, digits_) - &digits_[0]) {}
  AlphaNum(long long int i64)  // NOLINT(runtime/explicit)
      : piece_(digits_, FastInt64ToBufferLeft(i64, digits_) - &digits_[0]) {}
  AlphaNum(unsigned long long int u64)  // NOLINT(runtime/explicit)
      : piece_(digits_, FastUInt64ToBufferLeft(u64, digits_) - &digits_[0]) {}

  AlphaNum(float f)  // NOLINT(runtime/explicit)
      : piece_(digits_, strlen(FloatToBuffer(f, digits_))) {}
  AlphaNum(double f)  // NOLINT(runtime/explicit)
      : piece_(digits_, strlen(DoubleToBuffer(f, digits_))) {}

  AlphaNum(Hex hex);  // NOLINT(runtime/explicit)

  AlphaNum(const char *c_str) : piece_(c_str) {}   // NOLINT(runtime/explicit)
  AlphaNum(const StringPiece &pc) : piece_(pc) {}  // NOLINT(runtime/explicit)
  AlphaNum(const string &str)          // NOLINT(runtime/explicit)
      : piece_(str) {}

  StringPiece::size_type size() const { return piece_.size(); }
  const char *data() const { return piece_.data(); }
  StringPiece Piece() const { return piece_; }

 private:
  StringPiece piece_;
  char digits_[kFastToBufferSize];

  // Use ":" not ':'
  AlphaNum(char c);  // NOLINT(runtime/explicit)

  DISALLOW_COPY_AND_ASSIGN(AlphaNum);
};

extern AlphaNum gEmptyAlphaNum;

string StrCat(const AlphaNum &a) MUST_USE_RESULT;
string StrCat(const AlphaNum &a, const AlphaNum &b) MUST_USE_RESULT;
string StrCat(const AlphaNum &a, const AlphaNum &b,
              const AlphaNum &c) MUST_USE_RESULT;
string StrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
              const AlphaNum &d) MUST_USE_RESULT;

namespace internal {

// Do not call directly - this is not part of the public API.
string CatPieces(std::initializer_list<StringPiece> pieces);
void AppendPieces(string *dest, std::initializer_list<StringPiece> pieces);

}  // namespace internal

// Support 5 or more arguments
template <typename... AV>
string StrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
              const AlphaNum &d, const AlphaNum &e,
              const AV &... args) MUST_USE_RESULT;

template <typename... AV>
string StrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
              const AlphaNum &d, const AlphaNum &e, const AV &... args) {
  return internal::CatPieces({a.Piece(), b.Piece(), c.Piece(), d.Piece(),
                              e.Piece(),
                              static_cast<const AlphaNum &>(args).Piece()...});
}

void StrAppend(string *dest, const AlphaNum &a);
void StrAppend(string *dest, const AlphaNum &a, const AlphaNum &b);
void StrAppend(string *dest, const AlphaNum &a, const AlphaNum &b,
               const AlphaNum &c);
void StrAppend(string *dest, const AlphaNum &a, const AlphaNum &b,
               const AlphaNum &c, const AlphaNum &d);

template <typename... AV>
inline void StrAppend(string *dest, const AlphaNum &a, const AlphaNum &b,
                      const AlphaNum &c, const AlphaNum &d, const AlphaNum &e,
                      const AV &... args) {
  internal::AppendPieces(dest,
                         {a.Piece(), b.Piece(), c.Piece(), d.Piece(), e.Piece(),
                          static_cast<const AlphaNum &>(args).Piece()...});
}

}  // namespace strings
}  // namespace base

#endif  // BASE_STRINGS_STRCAT_H_
