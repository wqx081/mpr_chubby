#include "base/status.h"
#include <stdio.h>

namespace base {

Status::Status(base::error::Code code, StringPiece msg) {
  assert(code != base::error::OK);
  state_ = new State;
  state_->code = code;
  state_->msg = msg.ToString();
}

void Status::Update(const Status& new_status) {
  if (ok()) {
    *this = new_status;
  }
}

void Status::SlowCopyFrom(const State* src) {
  delete state_;
  if (src == nullptr) {
    state_ = nullptr;
  } else {
    state_ = new State(*src);
  }
}

const string& Status::empty_string() {
  static string* empty = new string;
  return *empty;
}

string Status::ToString() const {
  if (state_ == NULL) {
    return "OK";
  } else {
    char tmp[30];
    const char* type;
    switch (code()) {
      case base::error::CANCELLED:
        type = "Cancelled";
        break;
      case base::error::UNKNOWN:
        type = "Unknown";
        break;
      case base::error::INVALID_ARGUMENT:
        type = "Invalid argument";
        break;
      case base::error::DEADLINE_EXCEEDED:
        type = "Deadline exceeded";
        break;
      case base::error::NOT_FOUND:
        type = "Not found";
        break;
      case base::error::ALREADY_EXISTS:
        type = "Already exists";
        break;
      case base::error::PERMISSION_DENIED:
        type = "Permission denied";
        break;
      case base::error::UNAUTHENTICATED:
        type = "Unauthenticated";
        break;
      case base::error::RESOURCE_EXHAUSTED:
        type = "Resource exhausted";
        break;
      case base::error::FAILED_PRECONDITION:
        type = "Failed precondition";
        break;
      case base::error::ABORTED:
        type = "Aborted";
        break;
      case base::error::OUT_OF_RANGE:
        type = "Out of range";
        break;
      case base::error::UNIMPLEMENTED:
        type = "Unimplemented";
        break;
      case base::error::INTERNAL:
        type = "Internal";
        break;
      case base::error::UNAVAILABLE:
        type = "Unavailable";
        break;
      case base::error::DATA_LOSS:
        type = "Data loss";
        break;
      default:
        snprintf(tmp, sizeof(tmp), "Unknown code(%d)",
                 static_cast<int>(code()));
        type = tmp;
        break;
    }
    string result(type);
    result += ": ";
    result += state_->msg;
    return result;
  }
}

std::ostream& operator<<(std::ostream& os, const Status& x) {
  os << x.ToString();
  return os;
}

}  // namespace base
