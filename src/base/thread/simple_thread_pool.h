#ifndef BASE_THREAD_SIMPLE_THREAD_POOL_H_
#define BASE_THREAD_SIMPLE_THREAD_POOL_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

#include "base/thread/thread_pool_interface.h"
#include "base/thread/max_size_vector.h"
#include "base/thread/thread_environment.h"

namespace base {
namespace thread {

template<typename Environment>
class SimpleThreadPoolBase : public ThreadPoolInterface {
 public:
  explicit SimpleThreadPoolBase(int num_threads, Environment env = Environment()) 
      : env_(env),
        threads_(num_threads), 
        waiters_(num_threads) {
    // 创建num_threads 个线程，并且让它们执行 WorkerLoop()
    for (int i = 0; i < num_threads; ++i) {
      threads_.push_back(env.CreateThread([this, i]() {
        WorkerLoop(i);
      }));
    }
  }

  virtual ~SimpleThreadPoolBase() override {
    {
      std::unique_lock<std::mutex> l(mu_);
      while (!pending_.empty()) {
        empty_.wait(l);
      }
      exiting_ = true;

      for (auto w : waiters_) {
        w->ready = true;
        w->task.fn = nullptr;
        w->cv.notify_one();
      }
    }

    for (auto t : threads_) {
      delete t;
    }
  }

  virtual void Schedule(std::function<void()> fn) override {
    Task task = env_.CreateTask(std::move(fn));
    std::unique_lock<std::mutex> l(mu_);
    if (waiters_.empty()) {
      pending_.push_back(std::move(task));
    } else {
      Waiter* waiter = waiters_.back();
      waiters_.pop_back();
      waiter->ready = true;
      waiter->task = std::move(task);
      waiter->cv.notify_one();
    }
  }

  virtual int NumThreads() const override {
    return static_cast<int>(threads_.size());
  }

  virtual int CurrentThreadIndex() const override {
    const PerThread* per_thread = this->GetPerThread();
    if (per_thread->pool == this) {
      return per_thread->thread_index;
    } else {
      return -1;
    }
  }

 protected:
  void WorkerLoop(int thread_index) {

    std::unique_lock<std::mutex> l(mu_);

    PerThread* per_thread = GetPerThread();
    per_thread->pool = this;
    per_thread->thread_index = thread_index;
    Waiter waiter;
    Task task;
    while (!exiting_) { // Loop until existing_ be TRUE

      if (pending_.empty()) { // 没有任务，属于空闲状态
        waiter.ready = false;
        waiters_.push_back(&waiter);
        while (!waiter.ready) {
          waiter.cv.wait(l);
        }                      
        task = std::move(waiter.task);
        waiter.task.fn = nullptr;
      } else { // 处理一个 task
        task = std::move(pending_.front());
        pending_.pop_front();
        if (pending_.empty()) {
          empty_.notify_all();
        }          
      }                      

      if (task.fn) {
        mu_.unlock();
        env_.ExecuteTask(task);
        task.fn = nullptr;
        mu_.lock();
      }
    }
  }

 private:
  typedef typename Environment::Task Task;
  typedef typename Environment::EnvThread Thread;

  struct Waiter {
    std::condition_variable cv;
    Task task;
    bool ready;
  };  
  // 每个线程只有一份 PerThread
  struct PerThread {
    PerThread() : pool(nullptr), thread_index(-1) {}
    SimpleThreadPoolBase* pool;
    int thread_index;
  };

  Environment env_;
  std::mutex mu_;
  MaxSizeVector<Thread*> threads_;
  MaxSizeVector<Waiter*> waiters_;
  std::deque<Task> pending_;
  std::condition_variable empty_;
  bool exiting_ = false;

  PerThread* GetPerThread() const {
    static __thread PerThread per_thread;
    return &per_thread;
  }
};

using SimpleThreadPool = SimpleThreadPoolBase<STLThreadEnvironment>;

} // namespace thread
} // namespace base 
#endif // BASE_THREAD_SIMPLE_THREAD_POOL_H_
