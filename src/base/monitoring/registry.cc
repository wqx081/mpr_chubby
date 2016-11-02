#include "base/monitoring/registry.h"
#include "base/monitoring/metric.h"

using std::lock_guard;
using std::mutex;
using std::set;

namespace base {
namespace monitoring {

using std::lock_guard;
using std::mutex;
using std::set;

// static
Registry* Registry::Instance() {
  static Registry* registry(new Registry);
  return registry;
}


void Registry::AddMetric(const Metric* metric) {
  lock_guard<mutex> lock(mutex_);
  metrics_.insert(metric);
}


void Registry::ResetForTestingOnly() {
  lock_guard<mutex> lock(mutex_);
  metrics_.clear();
}


set<const Metric*> Registry::GetMetrics() const {
  lock_guard<mutex> lock(mutex_);
  return set<const Metric*>(metrics_);
}

} // namespace monitoring
} // namespace base
