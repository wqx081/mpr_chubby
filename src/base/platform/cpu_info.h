#ifndef BASE_PLATFORM_CPU_INFO_H_
#define BASE_PLATFORM_CPU_INFO_H_

namespace base {
namespace port {

static const bool kLittleEndian = true;

int NumSchedulableCPUs();

} // namespace port
} // namespace base 
#endif // BASE_PLATFORM_CPU_INFO_H_
