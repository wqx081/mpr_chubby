#ifndef BASE_THREAD_THREAD_POOL_INTERFACE_H_
#define BASE_THREAD_THREAD_POOL_INTERFACE_H_

#include <functional>

namespace base {
namespace thread {

class ThreadPoolInterface {
 public:
  virtual ~ThreadPoolInterface() {}

  virtual void Schedule(std::function<void()> fn) = 0;

  // 返回线程池中线程的总数
  virtual int NumThreads() const = 0;

  virtual int CurrentThreadIndex() const = 0;
};

} // namespace thread
} // namespace base
#endif // BASE_THREAD_THREAD_POOL_INTERFACE_H_
