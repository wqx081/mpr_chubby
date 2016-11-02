#ifndef BASE_MONITORING_MONITORING_H_
#define BASE_MONITORING_MONITORING_H_

#include <gflags/gflags.h>

#include "base/monitoring/counter.h"
#include "base/monitoring/gauge.h"

DECLARE_string(monitoring);

namespace base {
namespace monitoring {

const char kPrometheus[] = "prometheus";
const char kGcm[] = "gcm";


} // namespace monitoring
} // namespace base


#endif  // BASE_MONITORING_MONITORING_H_
