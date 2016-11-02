#ifndef BASE_GTL_PRIORITY_QUEUE_UTIL_H_
#define BASE_GTL_PRIORITY_QUEUE_UTIL_H_

#include <algorithm>
#include <queue>
#include <utility>

namespace base {
namespace gtl {

// Removes the top element from a std::priority_queue and returns it.
// Supports movable types.
template <typename T, typename Container, typename Comparator>
T ConsumeTop(std::priority_queue<T, Container, Comparator>* q) {
  // std::priority_queue is required to implement pop() as if it
  // called:
  //   std::pop_heap()
  //   c.pop_back()
  // unfortunately, it does not provide access to the removed element.
  // If the element is move only (such as a unique_ptr), there is no way to
  // reclaim it in the standard API.  std::priority_queue does, however, expose
  // the underlying container as a protected member, so we use that access
  // to extract the desired element between those two calls.
  using Q = std::priority_queue<T, Container, Comparator>;
  struct Expose : Q {
    using Q::c;
    using Q::comp;
  };
  auto& c = q->*&Expose::c;
  auto& comp = q->*&Expose::comp;
  std::pop_heap(c.begin(), c.end(), comp);
  auto r = std::move(c.back());
  c.pop_back();
  return r;
}

}  // namespace gtl
}  // namespace base

#endif  // BASE_GTL_PRIORITY_QUEUE_UTIL_H_
