#ifndef BASE_PLATFORM_LINUX_ERROR_H_
#define BASE_PLATFORM_LINUX_ERROR_H_

#include "base/status.h"

namespace base {

Status IOError(const string& context, int err_number);

}  // namespace base

#endif // BASE_PLATFORM_LINUX_ERROR_H_
