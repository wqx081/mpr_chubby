#ifndef BASE_THREADPOOL_H_
#define BASE_THREADPOOL_H_

#include <functional>
#include <memory>

#include "base/platform/env.h"
#include "base/macros.h"
#include "base/port.h"

namespace base {

class ThreadPool {
 public:
  ThreadPool(Env* env, const string& name, int num_threads);
  ThreadPool(Env* env, const ThreadOptions& thread_options,
             const string& name, int num_threads);
  ~ThreadPool();

  void Schedule(std::function<void()> fn);
  void ParallelFor(int64 total,
                  int64 cost_per_unit,
                  std::function<void(int64, int64)> fn);
  int NumThreads() const;
  int CurrentThreadIndex() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
  DISALLOW_COPY_AND_ASSIGN(ThreadPool);
};

} // namespace base
#endif // BASE_THREADPOOL_H_
