#ifndef BASE_STATUS_TEST_UTIL_H_
#define BASE_STATUS_TEST_UTIL_H_

#include "base/status.h"
#include <gtest/gtest.h>

// Macros for testing the results of functions that return base::Status.
#define MPR_EXPECT_OK(statement) \
  EXPECT_EQ(::base::Status::OK(), (statement))
#define MPR_ASSERT_OK(statement) \
  ASSERT_EQ(::base::Status::OK(), (statement))

#endif  // BASE_STATUS_TEST_UTIL_H_
