#ifndef BASE_CORE_ERRORS_H_
#define BASE_CORE_ERRORS_H_

#include "base/status.h"
#include "base/strings/strcat.h"
#include "base/logging.h"
#include "base/macros.h"

namespace base {
namespace errors {

typedef ::base::error::Code Code;

template <typename... Args>
void AppendToMessage(::base::Status* status, Args... args) {
  *status = ::base::Status(
      status->code(),
      ::base::strings::StrCat(status->error_message(), "\n\t", args...));
}

// For propagating errors when calling a function.
#define RETURN_IF_ERROR(expr)                            \
  do {                                                   \
    const ::base::Status _status = (expr);                \
    if (PREDICT_FALSE(!_status.ok())) return _status;    \
  } while (0)

#define RETURN_WITH_CONTEXT_IF_ERROR(expr, ...)                     \
  do {                                                              \
    ::base::Status _status = (expr);                                 \
    if (PREDICT_FALSE(!_status.ok())) {                             \
      ::base::errors::AppendToMessage(&_status, __VA_ARGS__);        \
      return _status;                                               \
    }                                                               \
  } while (0)


#define DECLARE_ERROR(FUNC, CONST)                                \
  template <typename... Args>                                     \
  ::base::Status FUNC(Args... args) {                              \
    return ::base::Status(::base::error::CONST,                     \
                                ::base::strings::StrCat(args...)); \
  }                                                               \
  inline bool Is##FUNC(const ::base::Status& status) {             \
    return status.code() == ::base::error::CONST;                  \
  }

DECLARE_ERROR(Cancelled, CANCELLED)
DECLARE_ERROR(InvalidArgument, INVALID_ARGUMENT)
DECLARE_ERROR(NotFound, NOT_FOUND)
DECLARE_ERROR(AlreadyExists, ALREADY_EXISTS)
DECLARE_ERROR(ResourceExhausted, RESOURCE_EXHAUSTED)
DECLARE_ERROR(Unavailable, UNAVAILABLE)
DECLARE_ERROR(FailedPrecondition, FAILED_PRECONDITION)
DECLARE_ERROR(OutOfRange, OUT_OF_RANGE)
DECLARE_ERROR(Unimplemented, UNIMPLEMENTED)
DECLARE_ERROR(Internal, INTERNAL)
DECLARE_ERROR(Aborted, ABORTED)
DECLARE_ERROR(DeadlineExceeded, DEADLINE_EXCEEDED)
DECLARE_ERROR(DataLoss, DATA_LOSS)
DECLARE_ERROR(Unknown, UNKNOWN)
DECLARE_ERROR(PermissionDenied, PERMISSION_DENIED)
DECLARE_ERROR(Unauthenticated, UNAUTHENTICATED)

#undef DECLARE_ERROR

// The CanonicalCode() for non-errors.
using ::base::error::OK;

}  // namespace errors
}  // namespace base

#endif  // BASE_ERRORS_H_
