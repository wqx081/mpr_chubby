#ifndef BASE_THREAD_MAX_SIZE_VECTOR_H_
#define BASE_THREAD_MAX_SIZE_VECTOR_H_

#include <stdint.h>

#include "base/logging.h"
#include "base/platform/mem.h"

namespace base {
namespace thread {

// T 必须符合:
// 默认构造函数
// 拷贝构造函数
// 赋值运算函数
//
template<typename T>
class MaxSizeVector {
 public:
  explicit MaxSizeVector(size_t n)
      : max_size_(n),
        size_(0),
        data_(static_cast<T*>(aligned_malloc(n * sizeof(T)))) {
    for (size_t i = 0; i < n; ++i) {
      new (&data_[i]) T;
    }  
  }
  
  MaxSizeVector(size_t n, const T& init_value)
      : max_size_(n),
        size_(0),
        data_(static_cast<T*>(aligned_malloc(n * sizeof(T)))) {
    for (size_t i = 0; i < n; ++i) {
      new (&data_[i]) T(init_value);
    } 
  }           
  
  // 析构函数会释放所有元素
  ~MaxSizeVector() {
    for (size_t i = 0; i < size_; ++i) {
      data_[i].~T();
    }
    aligned_free(data_);
  }

  void resize(size_t n) {
    CHECK(n <= max_size_);
    for (size_t i = size_; i < n; ++i) {
      new (&data_[i]) T;
    }
    for (size_t i = n; i < size_; ++i) {
      data_[i].~T();
    }
    size_ = n;
  } 

  void push_back(const T& v) {
    CHECK(size_ < max_size_);
    data_[size_++] = v;
  }
  
  const T& operator[] (size_t i) const {
    CHECK(i < size_);
    return data_[i];
  }  

  T& operator[] (size_t i) {
    CHECK(i < size_);
    return data_[i];
  }

  const T& back() const {
    CHECK(size_ > 0);
    return data_[size_ - 1];
  }

  T& back() {
    CHECK(size_ > 0);
    return data_[size_ - 1];
  }

  void pop_back() {
    CHECK(size_ > 0);
    size_--;
  }

  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  T* data() { return data_; }
  const T* data() const { return data_; }
  T* begin() { return data_; }
  T* end() { return data_ + size_; }
  const T* begin() const { return data_; }
  const T* end() const { return data_ + size_; }

 private:
  size_t max_size_;
  size_t size_;
  T* data_;
};

} // namespace thread
} // namespace base
#endif // BASE_THREAD_MAX_SIZE_VECTOR_H_
