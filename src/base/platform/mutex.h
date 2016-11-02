// Author: Wangqixiang
// Date: 2016/08/08
//
// 使用标准模板库的 Mutex, ConditionVariable.
#ifndef BASE_PLATFORM_MUTEX_H_
#define BASE_PLATFORM_MUTEX_H_

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "base/port.h"

namespace base {

enum ConditionResult {
  kCondTimeout,
  kCondMaybeNotified
};

using std::mutex;
using std::condition_variable;

// 简单封装封装成 RAII
class mutex_lock : public std::unique_lock<std::mutex> {
 public:
  mutex_lock(class mutex& m) : std::unique_lock<std::mutex>(m) {}
  mutex_lock(class mutex& m, std::try_to_lock_t t) : std::unique_lock<std::mutex>(m, t) {}
  mutex_lock(mutex_lock&& other) noexcept : std::unique_lock<std::mutex>(std::move(other)) {}
  ~mutex_lock() {}
};

inline ConditionResult WaitForMilliseconds(mutex_lock* mu,
                                           condition_variable* cv,
                                           int64 ms) {
  std::cv_status s = cv->wait_for(*mu,
                                  std::chrono::milliseconds(ms));
  return (s == std::cv_status::timeout) ? kCondTimeout : kCondMaybeNotified;
}

} // namespace base
#endif // BASE_PLATFORM_MUTEX_H_
