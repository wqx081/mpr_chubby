// Author: Wangqixiang (wangqx at mpreader dot com)
// Date: 2016/10/25

#ifndef BASE_REF_COUNTED_H_
#define BASE_REF_COUNTED_H_
#include <atomic>
#include "base/macros.h"

#include <glog/logging.h>

namespace base {

class RefCountedBase {
 public:
  RefCountedBase() : ref_(1) {
  }

  void Ref() const {
    DCHECK_GE(ref_.load(), 1);
    ref_.fetch_add(1, std::memory_order_relaxed);
  }

  bool Unref() const {
    DCHECK_GT(ref_.load(), 0);
    if (ref_.load(std::memory_order_acquire) == 1 ||
        ref_.fetch_sub(1) == 1) { // 对象拥有者为0, delete 对象
      DCHECK((ref_.store(0), true));
//      delete this;
      return true;
    } else {
      return false;
    }
  }

  bool RefCountIsOne() const {
    return (ref_.load(std::memory_order_acquire) == 1);
  }

 protected:
  virtual ~RefCountedBase() { } //DCHECK_EQ(ref_.load(), 0); }

 private:
  mutable std::atomic_int_fast32_t ref_;

  DISALLOW_COPY_AND_ASSIGN(RefCountedBase);
};

// 前置声明
template<typename T, typename Traits>
class RefCounted;

template<typename T>
struct DefaultRefCountedTraits {
  static void Destruct(const T* x) {
    RefCounted<T, DefaultRefCountedTraits>::DeleteInternal(x);
  }
};

template<typename T, typename Traits = DefaultRefCountedTraits<T>>
class RefCounted : public RefCountedBase {
 public:
  RefCounted() {}

  void Ref() const {
    RefCountedBase::Ref();
  }

  void Unref() const {
    if (RefCountedBase::Unref()) {
      Traits::Destruct(static_cast<const T*>(this));
    }
  }

 protected:
  ~RefCounted() {}

 private:
  friend struct DefaultRefCountedTraits<T>;
  static void DeleteInternal(const T* x) { delete x; }

  DISALLOW_COPY_AND_ASSIGN(RefCounted);
};

} // namespace base

template<typename T>
class scoped_refptr {
 public:
  typedef T element_type;

  scoped_refptr() : ptr_(nullptr) {}

  scoped_refptr(T* p) : ptr_(p) {
#if 0
    if (ptr_) {
      Ref(ptr_);
    }
#endif
  }

  scoped_refptr(const scoped_refptr<T>& r) : ptr_(r.ptr_) {
    if (ptr_) {
      Ref(ptr_);
    }
  }

  template<typename U,
           typename = typename std::enable_if<
               std::is_convertible<U*, T*>::value>::type>
  scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get()) {
    if (ptr_) {
      Ref(ptr_);
    }
  }

  scoped_refptr(scoped_refptr&& r) : ptr_(r.get()) {
    r.ptr_ = nullptr;
  }

  template<typename U,
           typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
  scoped_refptr(scoped_refptr<U>&& r) : ptr_(r.get()) {
    r.ptr_ = nullptr;
  }

  ~scoped_refptr() {
    if (ptr_) {
      Unref(ptr_);
    }
  }

  T* get() const { return ptr_; }
  T& operator*() const { assert(ptr_ != nullptr); return *ptr_; }
  T* operator->() const { assert(ptr_ != nullptr); return ptr_; }

  scoped_refptr<T>& operator=(T* p) {
    if (p) {
      Ref(p);
    } 
    T* old = ptr_;
    ptr_ = p;
    if (old) {
      Unref(old);
    }
    return *this;
  }

  scoped_refptr<T>& operator=(const scoped_refptr<T>& r) {
    return *this = r.ptr_;
  }

  template<typename U>
  scoped_refptr<T>& operator=(const scoped_refptr<U>& r) {
    return *this = r.get();
  }

  scoped_refptr<T>& operator=(const scoped_refptr<T>&& r) {
    scoped_refptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  template<typename U>
  scoped_refptr<T>& operator=(scoped_refptr<U>&& r) {
    scoped_refptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  void swap(T** pp) {
    T* p = ptr_;
    ptr_ = *pp;
    *pp = p;
  }

  void swap(scoped_refptr<T>& r) {
    swap(&r.ptr_);
  }

  explicit operator bool() const { return ptr_ != nullptr; }

  template<typename U>
  bool operator==(const scoped_refptr<U>& rhs) const {
    return ptr_ == rhs.get();
  }

  template<typename U>
  bool operator!=(const scoped_refptr<U>& rhs) const {
    return !operator==(rhs);
  }

  template<typename U>
  bool operator<(const scoped_refptr<U>& rhs) const {
    return ptr_ < rhs.get();
  }

 protected:
  T* ptr_;

 private:
  template<typename U>
  friend class scoped_refptr;

  static void Ref(T* ptr);
  static void Unref(T* ptr);
};

// static
template<typename T>
void scoped_refptr<T>::Ref(T* ptr) {
  ptr->Ref();
}

// static
template<typename T>
void scoped_refptr<T>::Unref(T* ptr) {
  ptr->Unref();
}

//

template <typename T>
scoped_refptr<T> make_scoped_refptr(T* t) {
  return scoped_refptr<T>(t);
}
  
template <typename T, typename U>
bool operator==(const scoped_refptr<T>& lhs, const U* rhs) {
  return lhs.get() == rhs;
}
  
template <typename T, typename U>
bool operator==(const T* lhs, const scoped_refptr<U>& rhs) {
  return lhs == rhs.get();
}
  
template <typename T>
bool operator==(const scoped_refptr<T>& lhs, std::nullptr_t null) {
  (void) null;
  return !static_cast<bool>(lhs);
}
  
template <typename T>
bool operator==(std::nullptr_t null, const scoped_refptr<T>& rhs) {
  (void) null;
  return !static_cast<bool>(rhs);
}
  
template <typename T, typename U>
bool operator!=(const scoped_refptr<T>& lhs, const U* rhs) {
  return !operator==(lhs, rhs);
}
  
template <typename T, typename U>
bool operator!=(const T* lhs, const scoped_refptr<U>& rhs) {
  return !operator==(lhs, rhs);
}
  
template <typename T>
bool operator!=(const scoped_refptr<T>& lhs, std::nullptr_t null) {
  return !operator==(lhs, null);
}
  
template <typename T>
bool operator!=(std::nullptr_t null, const scoped_refptr<T>& rhs) {
  return !operator==(null, rhs);
}
  
template <typename T>
std::ostream& operator<<(std::ostream& out, const scoped_refptr<T>& p) {
  return out << p.get();
}

#endif // BASE_REF_COUNTED_H_
