#ifndef BASE_MONITORING_PROMETHEUS_H_
#define BASE_MONITORING_PROMETHEUS_H_

#include <glog/logging.h>

#include "base/monitoring/util/protobuf.h"

namespace base {
namespace monitoring {

void ExportMetricsToPrometheus(std::ostream* os);
void ExportMetricsToHTML(std::ostream* os);

} // namespace monitoring
} // namespace base

#endif  // BASE_MONITORING_PROMETHEUS_H_
