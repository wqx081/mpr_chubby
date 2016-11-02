#ifndef BASE_PLATFORM_LINUX_LINUX_FILE_SYSTEM_H_
#define BASE_PLATFORM_LINUX_LINUX_FILE_SYSTEM_H_

#include "base/platform/env.h"

namespace base {

class LinuxFileSystem : public FileSystem {
 public:
  LinuxFileSystem() {}

  ~LinuxFileSystem() {}

  Status NewRandomAccessFile(
      const string& filename,
      std::unique_ptr<RandomAccessFile>* result) override;

  Status NewWritableFile(const string& fname,
                         std::unique_ptr<WritableFile>* result) override;

  Status NewAppendableFile(const string& fname,
                           std::unique_ptr<WritableFile>* result) override;

  Status NewReadOnlyMemoryRegionFromFile(
      const string& filename,
      std::unique_ptr<ReadOnlyMemoryRegion>* result) override;

  bool FileExists(const string& fname) override;
  bool SetNonBlocking(int fd) override;

  Status GetChildren(const string& dir, std::vector<string>* result) override;

  Status Stat(const string& fname, FileStatistics* stats) override;

  Status DeleteFile(const string& fname) override;

  Status CreateDirectory(const string& name) override;

  Status DeleteDirectory(const string& name) override;

  Status GetFileSize(const string& fname, uint64* size) override;

  Status RenameFile(const string& src, const string& target) override;
};

Status IOError(const string& context, int err_number);

class LocalLinuxFileSystem : public LinuxFileSystem {
 public:
  string TranslateName(const string& name) const override {
    StringPiece scheme, host, path;
    ParseURI(name, &scheme, &host, &path);
    return path.ToString();
  }
};

}  // namespace base

#endif // BASE_PLATFORM_LINUX_LINUX_FILE_SYSTEM_H_
