#ifndef BASE_HISTOGRAM_H_
#define BASE_HISTOGRAM_H_

#include <string>
#include <vector>
#include "base/gtl/array_slice.h"
#include "base/macros.h"
#include "base/platform/mutex.h"
#include "base/port.h"

namespace base {

class Histogram {
 public:
  // Create a histogram with a default set of bucket boundaries.
  // Buckets near zero cover very small ranges (e.g. 10^-12), and each
  // bucket range grows by ~10% as we head away from zero.  The
  // buckets cover the range from -DBL_MAX to DBL_MAX.
  Histogram();

  // Create a histogram with a custom set of bucket boundaries,
  // specified in "custom_bucket_limits[0..custom_bucket_limits.size()-1]"
  // REQUIRES: custom_bucket_limits[i] values are monotonically increasing.
  // REQUIRES: custom_bucket_limits is not empty()
  explicit Histogram(gtl::ArraySlice<double> custom_bucket_limits);

  ~Histogram() {}

  void Clear();
  void Add(double value);

  // Return the median of the values in the histogram
  double Median() const;

  // Return the "p"th percentile [0.0..100.0] of the values in the
  // distribution
  double Percentile(double p) const;

  // Return the average value of the distribution
  double Average() const;

  // Return the standard deviation of values in the distribution
  double StandardDeviation() const;

  // Returns a multi-line human-readable string representing the histogram
  // contents.  Example output:
  //   Count: 4  Average: 251.7475  StdDev: 432.02
  //   Min: -3.0000  Median: 5.0000  Max: 1000.0000
  //   ------------------------------------------------------
  //   [      -5,       0 )       1  25.000%  25.000% #####
  //   [       0,       5 )       1  25.000%  50.000% #####
  //   [       5,      10 )       1  25.000%  75.000% #####
  //   [    1000,   10000 )       1  25.000% 100.000% #####
  std::string ToString() const;

 private:
  double min_;
  double max_;
  double num_;
  double sum_;
  double sum_squares_;

  std::vector<double> custom_bucket_limits_;
  gtl::ArraySlice<double> bucket_limits_;
  std::vector<double> buckets_;

  double Remap(double x, double x0, double x1, double y0, double y1) const;

  DISALLOW_COPY_AND_ASSIGN(Histogram);
};

// Wrapper around a Histogram object that is thread safe.
//
// All methods hold a lock while delegating to a Histogram object owned by the
// ThreadSafeHistogram instance.
//
// See Histogram for documentation of the methods.
class ThreadSafeHistogram {
 public:
  ThreadSafeHistogram() {}
  explicit ThreadSafeHistogram(gtl::ArraySlice<double> custom_bucket_limits)
      : histogram_(custom_bucket_limits) {}

  ~ThreadSafeHistogram() {}

  void Clear();

  // TODO(touts): It might be a good idea to provide a AddN(<many values>)
  // method to avoid grabbing/releasing the lock when adding many values.
  void Add(double value);

  double Median() const;
  double Percentile(double p) const;
  double Average() const;
  double StandardDeviation() const;
  std::string ToString() const;

 private:
  mutable mutex mu_;
  Histogram histogram_; // GUARDED_BY(mu_);
};

}  // namespace base

#endif  // BASE_HISTOGRAM_H_
