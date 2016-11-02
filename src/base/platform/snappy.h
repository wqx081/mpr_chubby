#ifndef BASE_PLATFORM_SNAPPY_H_
#define BASE_PLATFORM_SNAPPY_H_

#include "base/port.h"

namespace base {
namespace port {

// Snappy compression/decompression support
bool Snappy_Compress(const char* input, size_t length, string* output);

bool Snappy_GetUncompressedLength(const char* input, size_t length,
                                  size_t* result);
bool Snappy_Uncompress(const char* input, size_t length, char* output);

}  // namespace port
}  // namespace base

#endif  //
