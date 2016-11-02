#ifndef BASE_GTL_MANUAL_CONSTRUCTOR_H_
#define BASE_GTL_MANUAL_CONSTRUCTOR_H_

#include <stddef.h>
#include <new>
#include <utility>

#include "base/macros.h"
#include "base/platform/mem.h"  // For aligned_malloc/aligned_free

namespace base {
namespace gtl {
namespace internal {

template <int alignment, int size>
struct AlignType {};
template <int size>
struct AlignType<0, size> {
  typedef char result[size];
};
#define LIB_GTL_ALIGN_ATTRIBUTE(X) __attribute__((aligned(X)))
#define LIB_GTL_ALIGN_OF(T) __alignof__(T)

#define LIB_GTL_ALIGNTYPE_TEMPLATE(X)                        \
  template <int size>                                        \
  struct AlignType<X, size> {                                \
    typedef LIB_GTL_ALIGN_ATTRIBUTE(X) char result[size];    \
  }

LIB_GTL_ALIGNTYPE_TEMPLATE(1);
LIB_GTL_ALIGNTYPE_TEMPLATE(2);
LIB_GTL_ALIGNTYPE_TEMPLATE(4);
LIB_GTL_ALIGNTYPE_TEMPLATE(8);
LIB_GTL_ALIGNTYPE_TEMPLATE(16);
LIB_GTL_ALIGNTYPE_TEMPLATE(32);
LIB_GTL_ALIGNTYPE_TEMPLATE(64);
LIB_GTL_ALIGNTYPE_TEMPLATE(128);
LIB_GTL_ALIGNTYPE_TEMPLATE(256);
LIB_GTL_ALIGNTYPE_TEMPLATE(512);
LIB_GTL_ALIGNTYPE_TEMPLATE(1024);
LIB_GTL_ALIGNTYPE_TEMPLATE(2048);
LIB_GTL_ALIGNTYPE_TEMPLATE(4096);
LIB_GTL_ALIGNTYPE_TEMPLATE(8192);

#define LIB_GTL_ALIGNED_CHAR_ARRAY(T, Size)                          \
  typename base::gtl::internal::AlignType<LIB_GTL_ALIGN_OF(T), \
                                         sizeof(T) * Size>::result

#undef LIB_GTL_ALIGNTYPE_TEMPLATE
#undef LIB_GTL_ALIGN_ATTRIBUTE

}  // namespace internal
}  // namespace gtl

template <typename Type>
class ManualConstructor {
 public:
  static void* operator new[](size_t size) {
    return aligned_malloc(size, LIB_GTL_ALIGN_OF(Type));
  }
  static void operator delete[](void* mem) { aligned_free(mem); }

  inline Type* get() { return reinterpret_cast<Type*>(space_); }
  inline const Type* get() const {
    return reinterpret_cast<const Type*>(space_);
  }

  inline Type* operator->() { return get(); }
  inline const Type* operator->() const { return get(); }

  inline Type& operator*() { return *get(); }
  inline const Type& operator*() const { return *get(); }

  inline void Init() { new (space_) Type; }

  template <typename... Ts>
  inline void Init(Ts&&... args) {                 // NOLINT
    new (space_) Type(std::forward<Ts>(args)...);  // NOLINT
  }
  inline void Destroy() { get()->~Type(); }

 private:
  LIB_GTL_ALIGNED_CHAR_ARRAY(Type, 1) space_;
};

#undef LIB_GTL_ALIGNED_CHAR_ARRAY
#undef LIB_GTL_ALIGN_OF

}  // namespace base

#endif  // BASE_GTL_MANUAL_CONSTRUCTOR_H_
