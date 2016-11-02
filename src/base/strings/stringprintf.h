// Usage:
//      string result = strings::Printf("%d %s\n", 10, "hello");
//      strings::SPrintf(&result, "%d %s\n", 10, "hello");
//      strings::Appendf(&result, "%d %s\n", 20, "there");

#ifndef BASE_STRINGS_STRINGPRINTF_H_
#define BASE_STRINGS_STRINGPRINTF_H_

#include <stdarg.h>
#include <string>

#include "base/macros.h"
#include "base/port.h"

namespace base {
namespace strings {

extern string SPrintf(const char* format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void Appendf(string* dst, const char* format, ...) PRINTF_ATTRIBUTE(2, 3);
extern void Appendv(string* dst, const char* format, va_list ap);

}  // namespace strings
}  // namespace base

#endif  // BASE_SRINGS_STRINGPRINTF_H_
