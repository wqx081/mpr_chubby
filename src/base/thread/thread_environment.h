#ifndef BASE_THREAD_THREAD_ENVIRONMENT_H_
#define BASE_THREAD_THREAD_ENVIRONMENT_H_

#include <functional>
#include <thread>

namespace base {
namespace thread {

struct STLThreadEnvironment {
  struct Task {
    std::function<void()> fn;
  };
  class EnvThread {
   public:
    EnvThread(std::function<void()> fn)
        : thread_(std::move(fn)) {}
    ~EnvThread() {
      thread_.join();
    }

   private:
    std::thread thread_;
  };

  EnvThread* CreateThread(std::function<void()> fn) {
    return new EnvThread(std::move(fn));
  }
  Task CreateTask(std::function<void()> fn) {
    return Task{std::move(fn)};
  }

  void ExecuteTask(const Task& t) {
    t.fn();
  }
};

} // namespace thread
} // namespace base

#endif // BASE_THREAD_THREAD_ENVIRONMENT_H_
