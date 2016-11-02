#include "base/thread/simple_thread_pool.h"

#include "base/logging.h"

#include <gtest/gtest.h>

namespace base {
namespace thread {

static const int kNumThreads = 30;

TEST(SimpleThreadPool, Basic) {
  for (int num_threads = 1; num_threads < kNumThreads; ++num_threads) {
    LOG(INFO) << "Testing with " << num_threads << " threads";
    SimpleThreadPool thread_pool(num_threads);
  }  
}

TEST(SimpleThreadPool, DoWork) {
  for (int num_threads = 1; num_threads < kNumThreads; ++num_threads) {
    LOG(INFO) << "Testing with " << num_threads << " threads";
    const int kWorkItem = 15;
    bool work[kWorkItem];
    for (int i = 0; i < kWorkItem; ++i) {
      work[i] = false;
    }
    {
      SimpleThreadPool pool(num_threads);
      for (int i = 0; i < kWorkItem; ++i) {
        pool.Schedule([&work, i]() {
          ASSERT_FALSE(work[i]);
          work[i] = true;
        });
      }
    }
 
    for (int i = 0; i < kWorkItem; ++i) {
      ASSERT_TRUE(work[i]);
    }
  }
}

} // namespace thread
} // namespace base
