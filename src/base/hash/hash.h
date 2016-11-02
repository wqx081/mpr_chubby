// Simple hash functions used for internal data structures

#ifndef TENSORFLOW_LIB_HASH_HASH_H_
#define TENSORFLOW_LIB_HASH_HASH_H_

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "base/port.h"

namespace base {
namespace hash {

extern uint32 Hash32(const char* data, size_t n, uint32 seed);
extern uint64 Hash64(const char* data, size_t n, uint64 seed);

inline uint64 Hash64(const char* data, size_t n) {
  return Hash64(data, n, 0xDECAFCAFFE);
}

inline uint64 Hash64(const string& str) {
  return Hash64(str.data(), str.size());
}

inline uint64 Hash64Combine(uint64 a, uint64 b) {
  return a ^ (b + 0x9e3779b97f4a7800ULL + (a << 10) + (a >> 4));
}

} // namespace hash
} // namespace base 

#endif
