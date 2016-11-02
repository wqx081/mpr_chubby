// Author: Wangqixiang (wangqx at mpreader.com)
// Date: 2016/08/08
//
// 模仿Golang 的 Slice, 主要用于函数的参数.
// 使用StringPiece时，必须确保它指向的外部存储没有被释放(销毁).
#ifndef BASE_STRINGPIECE_H_
#define BASE_STRINGPIECE_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <iosfwd>
#include <string>

#include "base/port.h"

namespace base {

class StringPiece {
 public:
  typedef size_t size_type;

  StringPiece() : data_(""), size_(0) {}
  StringPiece(const char* d, size_t n) : data_(d), size_(n) {}
  StringPiece(const string& s) : data_(s.data()), size_(s.size()) {}

  StringPiece(const char* s) : data_(s), size_(strlen(s)) {}

  void set(const void* data, size_t len) {
    data_ = reinterpret_cast<const char*>(data);
    size_ = len;
  }

  const char* data() const { return data_; }
  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }

  typedef const char* const_iterator;
  typedef const char* iterator;
  iterator begin() const { return data_; }
  iterator end() const { return data_ + size_; }

  static const size_t npos;

  char operator[](size_t n) const {
    assert(n < size());
    return data_[n];
  }

  void clear() {
    data_ = "";
    size_ = 0;
  }

  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  void remove_suffix(size_t n) {
    assert(size_ >= n);
    size_ -= n;
  }

  size_t find(char c, size_t pos = 0) const;
  size_t rfind(char c, size_t pos = npos) const;
  bool contains(StringPiece s) const;

  bool Consume(StringPiece x) {
    if (starts_with(x)) {
      remove_prefix(x.size_);
      return true;
    }
    return false;
  }

  StringPiece substr(size_t pos, size_t n = npos) const;

  //TODO(wqx): 支持哈希
#if 0
  struct Hasher {
    size_t operator()(StringPiece arg) const;
  };
#endif

  std::string ToString() const { return std::string(data_, size_); }
  int compare(StringPiece b) const;

  bool starts_with(StringPiece x) const {
    return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
  }
  bool ends_with(StringPiece x) const {
    return ((size_ >= x.size_) &&
            (memcmp(data_ + (size_ - x.size_), x.data_, x.size_) == 0));
  }

 private:
  const char* data_;
  size_t size_;

  // Default copyable
};

inline bool operator==(StringPiece x, StringPiece y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(StringPiece x, StringPiece y) { return !(x == y); }

inline bool operator<(StringPiece x, StringPiece y) { return x.compare(y) < 0; }
inline bool operator>(StringPiece x, StringPiece y) { return x.compare(y) > 0; }
inline bool operator<=(StringPiece x, StringPiece y) {
  return x.compare(y) <= 0;
}
inline bool operator>=(StringPiece x, StringPiece y) {
  return x.compare(y) >= 0;
}

inline int StringPiece::compare(StringPiece b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_)
      r = -1;
    else if (size_ > b.size_)
      r = +1;
  }
  return r;
}

// allow StringPiece to be logged
extern std::ostream& operator<<(std::ostream& o, base::StringPiece piece);

}  // namespace base
#endif  // BASE_STRINGPIECE_H_
