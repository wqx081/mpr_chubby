#ifndef BASE_STATUS_H_
#define BASE_STATUS_H_

#include <functional>
#include <iosfwd>
#include <string>

#include "base/macros.h"
#include "base/stringpiece.h"

#include <glog/logging.h>

namespace base {

namespace error {
enum Code {
  OK = 0,
  CANCELLED = 1,
  UNKNOWN = 2,
  INVALID_ARGUMENT = 3,
  DEADLINE_EXCEEDED = 4,
  NOT_FOUND = 5,
  ALREADY_EXISTS = 6,
  PERMISSION_DENIED = 7,
  UNAUTHENTICATED = 16,
  RESOURCE_EXHAUSTED = 8,
  FAILED_PRECONDITION = 9,
  ABORTED = 10,
  OUT_OF_RANGE = 11,
  UNIMPLEMENTED = 12,
  INTERNAL = 13,
  UNAVAILABLE = 14,
  DATA_LOSS = 15,
  DO_NOT_USE_RESERVED_FOR_FUTURE_EXPANSION_USE_DEFAULT_IN_SWITCH_INSTEAD_ = 20,
};
} // namespace errors

class Status {
 public:
  /// Create a success status.
  Status() : state_(NULL) {}
  ~Status() { delete state_; }

  Status(base::error::Code code, base::StringPiece msg);

  Status(const Status& s);
  void operator=(const Status& s);

  static Status OK() { return Status(); }

  bool ok() const { return (state_ == NULL); }

  base::error::Code code() const {
    return ok() ? base::error::OK : state_->code;
  }

  const string& error_message() const {
    return ok() ? empty_string() : state_->msg;
  }

  bool operator==(const Status& x) const;
  bool operator!=(const Status& x) const;

  void Update(const Status& new_status);
  string ToString() const;

 private:
  static const string& empty_string();
  struct State {
    base::error::Code code;
    string msg;
  };
  State* state_;

  void SlowCopyFrom(const State* src);
};

inline Status::Status(const Status& s)
    : state_((s.state_ == NULL) ? NULL : new State(*s.state_)) {}

inline void Status::operator=(const Status& s) {
  if (state_ != s.state_) {
    SlowCopyFrom(s.state_);
  }
}

inline bool Status::operator==(const Status& x) const {
  return (this->state_ == x.state_) || (ToString() == x.ToString());
}

inline bool Status::operator!=(const Status& x) const { return !(*this == x); }

std::ostream& operator<<(std::ostream& os, const Status& x);

typedef std::function<void(const Status&)> StatusCallback;

#define CHECK_OK(val) CHECK_EQ(::base::Status::OK(), (val))
#define QCHECK_OK(val) QCHECK_EQ(::base::Status::OK(), (val))

}  // namespace base 

#endif  // BASE_STATUS_H_
