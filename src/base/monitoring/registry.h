#ifndef BASE_MONITORING_REGISTRY_H_
#define BASE_MONITORING_REGISTRY_H_

#include <mutex>
#include <set>
#include <sstream>

#include "base/macros.h"

namespace base {
namespace monitoring {

class Metric;


class Registry {
 public:
  static Registry* Instance();

  void AddMetric(const Metric* metric);

  void ResetForTestingOnly();

  std::set<const Metric*> GetMetrics() const;

 private:
  Registry() = default;

  mutable std::mutex mutex_;
  std::set<const Metric*> metrics_;

  friend class RegistryTest;

  DISALLOW_COPY_AND_ASSIGN(Registry);
};

} // namespace monitoring 
} // namespace base

#endif  // BASE_MONITORING_REGISTRY_H_
