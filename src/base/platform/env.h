#ifndef BASE_PLATFORM_ENV_H_
#define BASE_PLATFORM_ENV_H_

#include <stdint.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/errors.h"
#include "base/status.h"
#include "base/stringpiece.h"
#include "base/platform/file_system.h"
#include "base/macros.h"
#include "base/platform/mutex.h"
#include "base/port.h"

namespace base {

class Thread;
struct ThreadOptions;

// 使用单例设计模式, 确保Default() 调用时，返回默认注册的 Env
class Env {
 public:
  Env();
  virtual ~Env() = default;

  static Env* Default();
  // FileSystem
  virtual Status GetFileSystemForFile(const string& fname, FileSystem** result);
  virtual Status GetRegisteredFileSystemSchemes(std::vector<string>* schemes);
  virtual Status RegisterFileSystem(const string& scheme,
                                    FileSystemRegistry::Factory factory);
  Status NewRandomAccessFile(const string& fname,
                             std::unique_ptr<RandomAccessFile>* result);
  Status NewWritableFile(const string& fname,
                         std::unique_ptr<WritableFile>* result);
  Status NewAppendableFile(const string& fname,
                           std::unique_ptr<WritableFile>* result);
  Status NewReadOnlyMemoryRegionFromFile(const string& fname,
                                         std::unique_ptr<ReadOnlyMemoryRegion>* result);

  bool FileExists(const string& fname);
  bool SetNonBlocking(int fd);

  Status GetChildren(const string& dir, std::vector<string>* result);
  virtual bool MatchPath(const string& path, const string& pattern) = 0;
  virtual Status GetMatchingPaths(const string& pattern,
                                  std::vector<string>* results);

  Status CreateDirectory(const string& dirname);
  Status CreateDirectoryRecursively(const string& dirname);

  Status DeleteFile(const string& fname);
  Status DeleteDirectory(const string& dirname);
  Status DeleteDirectoryRecursively(const string& dirname,
                                    int64* undeleted_files,
                                    int64* undeleted_dirs);
  
  Status Stat(const string& fname, FileStatistics* stat);

  Status IsDirectory(const string& fname);
  Status GetFileSize(const string& fname, uint64* file_size);
  Status RenameFile(const string& src, const string& dst);

  // Time
  virtual uint64 NowMicros() = 0;
  virtual uint64 NowSeconds() { return NowMicros() / 1000000L; }
  virtual void SleepForMicroseconds(int64 micros) = 0;

  // Thread
  virtual Thread* StartThread(const ThreadOptions& thread_options,
                              const string& name,
                              std::function<void()> fn) = 0;
  virtual void SchedClosure(std::function<void()> closure) = 0;
  virtual void SchedClosureAfter(int64 micros,
                                 std::function<void()> closure) = 0;

  virtual Status LoadLibrary(const char* library_filename, void** handle) = 0;
  virtual Status GetSymbolFromLibrary(void* handle,
                                      const char* symbol_name,
                                      void** symbol) = 0;

 private:
  std::unique_ptr<FileSystemRegistry> file_system_registry_;

  DISALLOW_COPY_AND_ASSIGN(Env);
};

// 简化我们继承Env时，只覆盖部分功能
class EnvDecorator: public Env {
 public:
  explicit EnvDecorator(Env* t) : target_(t) {}  
  virtual ~EnvDecorator();

  Env* target() const { return target_; }

  Status GetFileSystemForFile(const string& fname,
                              FileSystem** result) override {
    return target_->GetFileSystemForFile(fname, result);
  }

  Status GetRegisteredFileSystemSchemes(std::vector<string>* schemes) override {
    return target_->GetRegisteredFileSystemSchemes(schemes);
  }

  Status RegisterFileSystem(const string& scheme,
                            FileSystemRegistry::Factory factory) override {
    return target_->RegisterFileSystem(scheme, factory);
  }

  bool MatchPath(const string& path, const string& pattern) override {
    return target_->MatchPath(path, pattern);
  }

  uint64 NowMicros() override { return target_->NowMicros(); }

  void SleepForMicroseconds(int64 micros) override {
    target_->SleepForMicroseconds(micros);
  }

  Thread* StartThread(const ThreadOptions& thread_options, const string& name,
                      std::function<void()> fn) override {
    return target_->StartThread(thread_options, name, fn);
  }

  void SchedClosure(std::function<void()> closure) override {
    target_->SchedClosure(closure);
  }

  void SchedClosureAfter(int64 micros, std::function<void()> closure) override {
    target_->SchedClosureAfter(micros, closure);
  }

  Status LoadLibrary(const char* library_filename, void** handle) override {
    return target_->LoadLibrary(library_filename, handle);
  }

  Status GetSymbolFromLibrary(void* handle, const char* symbol_name,
                              void** symbol) override {
    return target_->GetSymbolFromLibrary(handle, symbol_name, symbol);
  }

 private:
  Env* target_;
};

// Thread

class Thread {
 public:
  Thread() {}
  virtual ~Thread();

 private:  
  DISALLOW_COPY_AND_ASSIGN(Thread);
};

struct ThreadOptions {
  size_t stack_size = 0;
  size_t guard_size = 0;
};

// Utilities
Status ReadFileToString(Env* env, const string& fname, string* data);
Status WriteStringToFile(Env* env, const string& fname, const StringPiece& data);

namespace register_file_system {

template<typename Factory>
struct Register {
  Register(Env* env, const string& scheme) {
    env->RegisterFileSystem(scheme,
                            []() -> FileSystem* { return new Factory; });
  }
};

} // namespace register_file_system

} // namespace base


#define REGISTER_FILE_SYSTEM_ENV(env, scheme, factory) \
  REGISTER_FILE_SYSTEM_UNIQ_HELPER(__COUNTER__, env, scheme, factory)
#define REGISTER_FILE_SYSTEM_UNIQ_HELPER(ctr, env, scheme, factory) \
  REGISTER_FILE_SYSTEM_UNIQ(ctr, env, scheme, factory)
#define REGISTER_FILE_SYSTEM_UNIQ(ctr, env, scheme, factory)   \
  static ::base::register_file_system::Register<factory>        \
      register_ff##ctr ATTRIBUTE_UNUSED =                      \
          ::base::register_file_system::Register<factory>(env, scheme)

#define REGISTER_FILE_SYSTEM(scheme, factory) \
  REGISTER_FILE_SYSTEM_ENV(Env::Default(), scheme, factory);

#endif // BASE_PLATFORM_ENV_H_
