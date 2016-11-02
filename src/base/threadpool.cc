#include "base/threadpool.h"

#include "base/thread/simple_thread_pool.h"
#include "base/thread/device_thread_pool.h"
#include "base/thread/cost_model.h"

#include "base/platform/mutex.h"
#include "base/port.h"

namespace base {

struct SimpleEnvironment {
  typedef Thread EnvThread;
  struct TaskImpl {
    std::function<void()> fn;
  };
  struct Task {
    std::unique_ptr<TaskImpl> fn;
  };

  Env* const env_;
  const ThreadOptions thread_options_;
  const string name_;

  SimpleEnvironment(Env* env, const ThreadOptions& thread_options, const string& name)
      : env_(env), thread_options_(thread_options), name_(name) {}

  EnvThread* CreateThread(std::function<void()> fn) {
    return env_->StartThread(thread_options_, name_, [=]() {
      fn();
    });
  }

  Task CreateTask(std::function<void()> fn) {
    return Task{
      std::unique_ptr<TaskImpl>(new TaskImpl{
        std::move(fn)
      }),
    };
  }

  void ExecuteTask(const Task& t) {
    t.fn->fn();
  }
};

//////////////

struct ThreadPool::Impl : thread::SimpleThreadPoolBase<SimpleEnvironment> {
  Impl(Env* env, const ThreadOptions& thread_options, const string& name, int num_threads) 
        : thread::SimpleThreadPoolBase<SimpleEnvironment>(num_threads, 
                                                          SimpleEnvironment(env, thread_options, name)) {}
  void ParallelFor(int64 total,
                   int64 cost_per_unit,
                   std::function<void(int64, int64)> fn) {
    CHECK_GE(total, 0);
    CHECK_EQ(total, (int64)(thread::Index)total);
    thread::DeviceThreadPool device(this, this->NumThreads());
    device.ParallelFor(total, thread::OperatorCost(0, 0, cost_per_unit),
                       [&fn](thread::Index first, thread::Index last) { fn(first, last); });
  }

};

ThreadPool::ThreadPool(Env* env, const string& name, int num_threads)
    : ThreadPool(env, ThreadOptions(), name, num_threads) {}


ThreadPool::ThreadPool(Env* env, const ThreadOptions& thread_options,
                       const string& name, int num_threads) {
  CHECK_GE(num_threads, 1);
  impl_.reset(
    new ThreadPool::Impl(env, thread_options, "mpr_thread_" + name, num_threads));
}

ThreadPool::~ThreadPool() {}

void ThreadPool::Schedule(std::function<void()> fn) {
  CHECK(fn != nullptr);
  impl_->Schedule(std::move(fn));
} 
  
void ThreadPool::ParallelFor(int64 total, int64 cost_per_unit,
                             std::function<void(int64, int64)> fn) {
  impl_->ParallelFor(total, cost_per_unit, std::move(fn));
}
  
int ThreadPool::NumThreads() const { return impl_->NumThreads(); }
int ThreadPool::CurrentThreadIndex() const { return impl_->CurrentThreadIndex(); }

} // namespace base
