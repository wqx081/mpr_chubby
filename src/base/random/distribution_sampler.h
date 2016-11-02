// DistributionSampler allows generating a discrete random variable with a given
// distribution.
// The values taken by the variable are [0, N) and relative weights for each
// value are specified using a vector of size N.
//
// The Algorithm takes O(N) time to precompute data at construction time and
// takes O(1) time (2 random number generation, 2 lookups) for each sample.
// The data structure takes O(N) memory.
//
// In contrast, util/random/weighted-picker.h provides O(lg N) sampling.
// The advantage of that implementation is that weights can be adjusted
// dynamically, while DistributionSampler doesn't allow weight adjustment.
//
// The algorithm used is Walker's Aliasing algorithm, described in Knuth, Vol 2.

#ifndef BASE_RANDOM_DISTRIBUTION_SAMPLER_H_
#define BASE_RANDOM_DISTRIBUTION_SAMPLER_H_

#include <memory>
#include <utility>

#include "base/gtl/array_slice.h"
#include "base/random/simple_philox.h"
#include "base/logging.h"
#include "base/macros.h"

namespace base {
namespace random {

class DistributionSampler {
 public:
  explicit DistributionSampler(const gtl::ArraySlice<float>& weights);

  ~DistributionSampler() {}

  int Sample(SimplePhilox* rand) const {
    float r = rand->RandFloat();
    // Since n is typically low, we don't bother with UnbiasedUniform.
    int idx = rand->Uniform(num_);
    if (r < prob(idx)) return idx;
    // else pick alt from that bucket.
    DCHECK_NE(-1, alt(idx));
    return alt(idx);
  }

  int num() const { return num_; }

 private:
  float prob(int idx) const {
    DCHECK_LT(idx, num_);
    return data_[idx].first;
  }

  int alt(int idx) const {
    DCHECK_LT(idx, num_);
    return data_[idx].second;
  }

  void set_prob(int idx, float f) {
    DCHECK_LT(idx, num_);
    data_[idx].first = f;
  }

  void set_alt(int idx, int val) {
    DCHECK_LT(idx, num_);
    data_[idx].second = val;
  }

  int num_;
  std::unique_ptr<std::pair<float, int>[]> data_;

  DISALLOW_COPY_AND_ASSIGN(DistributionSampler);
};

}  // namespace random
}  // namespace base
#endif  // BASE_RANDOM_DISTRIBUTION_SAMPLER_H_
