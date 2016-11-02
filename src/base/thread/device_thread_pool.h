#ifndef BASE_THREAD_DEVICE_THREAD_POOL_H_
#define BASE_THREAD_DEVICE_THREAD_POOL_H_

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "base/logging.h"
#include "base/thread/thread_pool_interface.h"
#include "base/thread/simple_thread_pool.h"
#include "base/thread/cost_model.h"

namespace base {
namespace thread {

typedef int Index;

namespace {

template <typename T, typename X, typename Y>
inline T divup(const X x, const Y y) {
    return static_cast<T>((x + y - 1) / y);
}

template <typename T>
inline T divup(const T x, const T y) {
  return static_cast<T>((x + y - 1) / y);
}


} // namespace

template<typename Env> using ThreadPoolBase = SimpleThreadPoolBase<Env>;
typedef SimpleThreadPool ThreadPool;


class Barrier {
 public:
  Barrier(unsigned int count)
      : state_(count << 1),
        notified_(false) {}
  ~Barrier() {
    DCHECK((state_ >> 1) == 0);
  }

  void Notify() {
    unsigned int v = state_.fetch_sub(2, std::memory_order_acq_rel) - 2;
    if (v != 1) {
      assert(((v + 2) & ~1) != 0);
      return;  // either count has not dropped to 0, or waiter is not waiting
    }
    std::unique_lock<std::mutex> l(mu_);
    assert(!notified_);
    notified_ = true;
    cv_.notify_all();
  }

  void Wait() {
    unsigned int v = state_.fetch_or(1, std::memory_order_acq_rel);
    if ((v >> 1) == 0) return;
    std::unique_lock<std::mutex> l(mu_);
    while (!notified_) {
      cv_.wait(l);
    }
  }

 private:
  std::mutex mu_;
  std::condition_variable cv_;
  std::atomic<unsigned> state_;
  bool notified_;
};

class Notification : public Barrier {
  Notification() : Barrier(1) {
  }
};

//
template<typename Function, typename... Args>
struct FunctionWrapperWithNotification {
  static void Run(Notification* n, Function f, Args... args) {
    f(args...);
    if (n) {
      n->Notify();
    }
  }
};

template<typename Function, typename... Args>
struct FunctionWrapperWithBarrier {
  static void Run(Barrier* b, Function f, Args... args) {
    f(args...);
    if (b) {
      b->Notify();
    }
  }
};

template<typename SyncT>
static inline void WaitUntilReady(SyncT* n) {
  if (n) {
    n->Wait();
  }
}

class DeviceThreadPool {
 public:

  DeviceThreadPool(ThreadPoolInterface* pool, int num_cores) : pool_(pool), num_threads_(num_cores) {}
  
  int NumThreads() const {
    return num_threads_;
  } 

  int CurrentThreadIndex() const {
    return pool_->CurrentThreadIndex();
  }

  template<typename Function, typename... Args>
  Notification* Enqueue(Function&& f, Args&&... args) const {
    Notification* n = new Notification();
    pool_->Schedule(std::bind(&FunctionWrapperWithNotification<Function, Args...>::Run,
                              n,
                              f,
                              args...));
    return n;
  }
  
  template<typename Function, typename... Args>
  void EnqueueWithBarrier(Barrier* barrier,
                          Function&& function,
                          Args&&... args) const {
    pool_->Schedule(std::bind(&FunctionWrapperWithBarrier<Function, Args...>::Run,
                              barrier,
                              function,
                              args...));
  }

  template<typename Function, typename... Args>
  void EnqueueWithoutNotification(Function&& function, Args&&... args) const {
    pool_->Schedule(std::bind(function, args...));
  }

  //////
  //
  void ParallelFor(Index n, const OperatorCost& cost,
                   std::function<Index(Index)> block_align,
                   std::function<void(Index, Index)> f) const {

    typedef CostModel<DeviceThreadPool> DeviceCostModel;
    if (n <= 1 || NumThreads() == 1 ||
        DeviceCostModel::NumThreads(n, cost, static_cast<int>(NumThreads())) == 1) {
      f(0, n);
      return;
    }

    double block_size_f = 1.0 / DeviceCostModel::TaskSize(1, cost);
    Index block_size = std::min(n,
                                std::max<Index>(1, block_size_f));
    const Index max_block_size = std::min(n,
                                          std::max<Index>(1, 2 * block_size_f));
    if (block_size) {
      Index new_block_size = block_align(block_size);
      assert(new_block_size >= block_size);
      block_size = std::min(n, new_block_size);
    }
    Index block_count = divup(n, block_size);
    double max_efficiency =
        static_cast<double>(block_count) /
        (divup<int>(block_count, NumThreads()) * NumThreads());

    for (Index prev_block_count = block_count; prev_block_count > 1;) {
      Index coarser_block_size = divup(n, prev_block_count - 1);
      if (block_align) {
        Index new_block_size = block_align(coarser_block_size);
        assert(new_block_size >= coarser_block_size);
        coarser_block_size = std::min(n, new_block_size);
      } 
      if (coarser_block_size > max_block_size) {
        break;  // Reached max block size. Stop.
      } 
      const Index coarser_block_count = divup(n, coarser_block_size);
      assert(coarser_block_count < prev_block_count);
      prev_block_count = coarser_block_count;
      const double coarser_efficiency =
          static_cast<double>(coarser_block_count) /
          (divup<int>(coarser_block_count, NumThreads()) * NumThreads());
      if (coarser_efficiency + 0.01 >= max_efficiency) { 
        block_size = coarser_block_size;
        block_count = coarser_block_count;
        if (max_efficiency < coarser_efficiency) {
          max_efficiency = coarser_efficiency;
        } 
      } 
    } 
    
    Barrier barrier(static_cast<unsigned int>(block_count));
    std::function<void(Index, Index)> HandleRange;
    HandleRange = [=, &HandleRange, &barrier, &f](Index first, Index last) {
      if (last - first <= block_size) {
        f(first, last);
        barrier.Notify();
        return;
      }
      Index mid = first + divup((last - first) / 2, block_size) * block_size;
      pool_->Schedule([=, &HandleRange]() { HandleRange(mid, last); });
      pool_->Schedule([=, &HandleRange]() { HandleRange(first, mid); });
    };
    HandleRange(0, n);
    barrier.Wait();
  }

  void ParallelFor(Index n, const OperatorCost& cost, 
                   std::function<void(Index, Index)> f) const {
    ParallelFor(n, cost, nullptr, std::move(f));
  }

 private:
  ThreadPoolInterface* pool_;
  int num_threads_;
};

} // namespace thread
} // namespace base
#endif // BASE_THREAD_DEVICE_THREAD_POOL_H_
