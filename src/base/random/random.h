#ifndef BASE_RANDOM_RANDOM_H_
#define BASE_RANDOM_RANDOM_H_

#include "base/port.h"

namespace base {
namespace random {

// Return a 64-bit random value.  Different sequences are generated
// in different processes.
uint64 New64();

}  // namespace random
}  // namespace base

#endif  // BASE_RANDOM_RANDOM_H_
