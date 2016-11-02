#include <gflags/gflags.h>

#include "base/monitoring/monitoring.h"

DEFINE_string(monitoring, "prometheus",
              "Which monitoring system to use, current only supported prometheus");
