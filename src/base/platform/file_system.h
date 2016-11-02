#ifndef BASE_PLATFORM_FILE_SYSTEM_H_
#define BASE_PLATFORM_FILE_SYSTEM_H_

#include <stdint.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "base/errors.h"
#include "base/status.h"
#include "base/stringpiece.h"
#include "base/platform/file_statistics.h"
#include "base/macros.h"
#include "base/port.h"

namespace base {

class RandomAccessFile;
class WritableFile;
class ReadOnlyMemoryRegion;

class FileSystem {
 public:
  FileSystem() {}
  virtual ~FileSystem();

  virtual Status NewRandomAccessFile(const string& fname,
                                     std::unique_ptr<RandomAccessFile>* result) = 0;
  virtual Status NewWritableFile(const string& fname,
                                 std::unique_ptr<WritableFile>* result) = 0;
  virtual Status NewAppendableFile(const string& fname,
                                   std::unique_ptr<WritableFile>* result) = 0;
  virtual Status NewReadOnlyMemoryRegionFromFile(const string& fname,
                                                 std::unique_ptr<ReadOnlyMemoryRegion>* result) = 0;
  

  //
  virtual bool FileExists(const string& fname) = 0;
  // TODO(wqx)
  virtual bool SetNonBlocking(int fd) = 0;
  virtual Status GetChildren(const string& dir, std::vector<string>* result) = 0;
  virtual Status GetMatchingPaths(const string& pattern, std::vector<string>* results);
  virtual Status Stat(const string& fname, FileStatistics* stat) = 0;
  virtual Status DeleteFile(const string& fname) = 0;

  virtual Status CreateDirectory(const string& dirname) = 0;
  virtual Status CreateDirectoryRecursively(const string& dirname);

  virtual Status DeleteDirectory(const string& dirname) = 0;
  virtual Status DeleteDirectoryRecursively(const string& dirname,
                                            int64* undeleted_files,
                                            int64* undeleted_dirs);

  virtual Status GetFileSize(const string& fname, uint64* file_size) = 0;
  virtual Status RenameFile(const string& src, const string& target) = 0;
  virtual Status IsDirectory(const string& fname);
  virtual string TranslateName(const string& name) const;
};

////////////
class RandomAccessFile {
 public:
  RandomAccessFile() {}
  virtual ~RandomAccessFile();
  
  virtual Status Read(uint64 offset, size_t n, StringPiece* result,
                      char* scratch) const = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(RandomAccessFile);
};

class WritableFile {
 public:
  WritableFile() {}
  virtual ~WritableFile();

  virtual Status Append(const StringPiece& data) = 0;
  virtual Status Close() = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(WritableFile);
};

class ReadOnlyMemoryRegion {
 public:
  ReadOnlyMemoryRegion() {}
  virtual ~ReadOnlyMemoryRegion() {}
  virtual const void* data() = 0;
  virtual uint64 length() = 0;
};

class FileSystemRegistry {
 public:
  typedef std::function<FileSystem*()> Factory;
 
  virtual ~FileSystemRegistry();
  virtual Status Register(const string& scheme, Factory factory) = 0;
  virtual FileSystem* Lookup(const string& scheme) = 0;
  virtual Status GetRegisteredFileSystemSchemes(std::vector<string>* schemes) = 0;
};

void ParseURI(StringPiece uri, StringPiece* scheme, StringPiece* host, StringPiece* path);
string CreateURI(StringPiece scheme, StringPiece host, StringPiece path);

} // namespace base
#endif // BASE_PLATFORM_FILE_SYSTEM_H_
