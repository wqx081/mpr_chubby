#include "base/random/random.h"

#include <random>
#include "base/platform/mutex.h"
#include "base/port.h"


namespace base {
namespace random {

std::mt19937_64* InitRng() {
  std::random_device device("/dev/urandom");
  return new std::mt19937_64(device());
}

uint64 New64() {
  static std::mt19937_64* rng = InitRng();
  static mutex mu;
  mutex_lock l(mu);
  return (*rng)();
}

}  // namespace random
}  // namespace base
