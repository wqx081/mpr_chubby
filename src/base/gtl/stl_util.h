#ifndef BASE_GTL_STL_UTIL_H_
#define BASE_GTL_STL_UTIL_H_

#include <stddef.h>
#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/port.h"

namespace base {
namespace gtl {

inline char* string_as_array(string* str) {
  return str->empty() ? NULL : &*str->begin();
}

template <typename T, typename Allocator>
inline T* vector_as_array(std::vector<T, Allocator>* v) {
#if defined NDEBUG && !defined _GLIBCXX_DEBUG
  return &*v->begin();
#else
  return v->empty() ? NULL : &*v->begin();
#endif
}
// vector_as_array overload for const std::vector<>.
template <typename T, typename Allocator>
inline const T* vector_as_array(const std::vector<T, Allocator>* v) {
#if defined NDEBUG && !defined _GLIBCXX_DEBUG
  return &*v->begin();
#else
  return v->empty() ? NULL : &*v->begin();
#endif
}

inline void STLStringResizeUninitialized(string* s, size_t new_size) {
  s->resize(new_size);
}

// Calls delete (non-array version) on the SECOND item (pointer) in each pair in
// the range [begin, end).
//
// Note: If you're calling this on an entire container, you probably want to
// call STLDeleteValues(&container) instead, or use ValueDeleter.
template <typename ForwardIterator>
void STLDeleteContainerPairSecondPointers(ForwardIterator begin,
                                          ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete temp->second;
  }
}

// Deletes all the elements in an STL container and clears the container. This
// function is suitable for use with a vector, set, hash_set, or any other STL
// container which defines sensible begin(), end(), and clear() methods.
//
// If container is NULL, this function is a no-op.
template <typename T>
void STLDeleteElements(T* container) {
  if (!container) return;
  auto it = container->begin();
  while (it != container->end()) {
    auto temp = it;
    ++it;
    delete *temp;
  }
  container->clear();
}

// Given an STL container consisting of (key, value) pairs, STLDeleteValues
// deletes all the "value" components and clears the container. Does nothing in
// the case it's given a NULL pointer.
template <typename T>
void STLDeleteValues(T* container) {
  if (!container) return;
  auto it = container->begin();
  while (it != container->end()) {
    auto temp = it;
    ++it;
    delete temp->second;
  }
  container->clear();
}

// Sorts and removes duplicates from a sequence container.
template <typename T>
inline void STLSortAndRemoveDuplicates(T* v) {
  std::sort(v->begin(), v->end());
  v->erase(std::unique(v->begin(), v->end()), v->end());
}

}  // namespace gtl
}  // namespace base

#endif  // BASE_GTL_STL_UTIL_H_
