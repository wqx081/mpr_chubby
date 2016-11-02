#ifndef BASE_NOTIFICATION_H_
#define BASE_NOTIFICATION_H_

#include <assert.h>
#include <chrono>              // NOLINT
#include <condition_variable>  // NOLINT

#include "base/platform/mutex.h"
#include "base/port.h"

namespace base {

class Notification {
 public:
  Notification() : notified_(false) {}
  ~Notification() {}

  void Notify() {
    mutex_lock l(mu_);
    assert(!notified_);
    notified_ = true;
    cv_.notify_all();
  }

  bool HasBeenNotified() {
    mutex_lock l(mu_);
    return notified_;
  }

  void WaitForNotification() {
    mutex_lock l(mu_);
    while (!notified_) {
      cv_.wait(l);
    }
  }

 private:
  friend bool WaitForNotificationWithTimeout(Notification* n,
                                             int64 timeout_in_ms);
  bool WaitForNotificationWithTimeout(int64 timeout_in_ms) {
    mutex_lock l(mu_);
    return cv_.wait_for(l, std::chrono::milliseconds(timeout_in_ms),
                        [this]() { return notified_; });
  }

  mutex mu_;
  condition_variable cv_;
  bool notified_;
};

inline bool WaitForNotificationWithTimeout(Notification* n,
                                           int64 timeout_in_ms) {
  return n->WaitForNotificationWithTimeout(timeout_in_ms);
}

}  // namespace base

#endif  // BASE_NOTIFICATION_H_
