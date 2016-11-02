#include "base/monitoring/registry.h"

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>

#include "base/monitoring/monitoring.h"

#include <gtest/gtest.h>

namespace base {
namespace monitoring {

using std::ostringstream;
using std::string;
using std::unique_ptr;
using std::set;
using testing::AllOf;
using testing::AnyOf;
using testing::Contains;


class RegistryTest : public ::testing::Test {
 public:
  void TearDown() {
    Registry::Instance()->ResetForTestingOnly();
  }

 protected:
  const set<const Metric*>& GetMetrics() {
    return Registry::Instance()->metrics_;
  }
};


TEST_F(RegistryTest, TestAddMetric) {
  unique_ptr<Counter<>> counter(Counter<>::New("name", "help"));
  unique_ptr<Gauge<>> gauge(Gauge<>::New("name", "help"));
  EXPECT_EQ(static_cast<size_t>(2), GetMetrics().size());
  EXPECT_THAT(GetMetrics(),
              AllOf(Contains(counter.get()), Contains(gauge.get())));
}

} // namespace monitoring
} // namespace base
