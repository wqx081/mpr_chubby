#include <deque>
#include <vector>

#include "base/port.h"
#include <google/protobuf/io/zero_copy_stream.h>

#include "base/errors.h"
#include "base/gtl/map_util.h"
#include "base/gtl/stl_util.h"
#include "base/io/path.h"
#include "base/platform/env.h"

namespace base {

class FileSystemRegistryImpl : public FileSystemRegistry {
 public:
  Status Register(const string& scheme, Factory factory) override;
  FileSystem* Lookup(const string& scheme) override;
  Status GetRegisteredFileSystemSchemes(std::vector<string>* schemes) override;

 private:
  mutable mutex mu_;
  mutable std::unordered_map<string, std::unique_ptr<FileSystem>> registry_;
};

Status FileSystemRegistryImpl::Register(const string& scheme,
                                        FileSystemRegistry::Factory factory) {
  mutex_lock lock(mu_);
  if (!registry_.emplace(string(scheme), std::unique_ptr<FileSystem>(factory())).second) {
    return errors::AlreadyExists("File factory for ", scheme, " already registered");
  }
  return Status::OK();
}

FileSystem* FileSystemRegistryImpl::Lookup(const string& scheme) {
  mutex_lock lock(mu_);
  const auto found = registry_.find(scheme);
  if (found == registry_.end()) {
    return nullptr;
  }
  return found->second.get();
}

Status FileSystemRegistryImpl::GetRegisteredFileSystemSchemes(
      std::vector<string>* schemes) {
  mutex_lock lock(mu_);
  for (const auto& e : registry_) {
    schemes->push_back(e.first);
  }
  return Status::OK();
}

//////////////////////////////

Env::Env() : file_system_registry_(new FileSystemRegistryImpl) {}
  
Status Env::GetFileSystemForFile(const string& fname, FileSystem** result) {
  (void) fname;
  StringPiece scheme, host, path;
  ParseURI(fname, &scheme, &host, &path);
  FileSystem* file_system = file_system_registry_->Lookup(scheme.ToString());
  if (!file_system) {
    return errors::Unimplemented("File system scheme ", scheme,
                                 " not implemented");
  }
  *result = file_system;
  return Status::OK();
}
  
Status Env::GetRegisteredFileSystemSchemes(std::vector<string>* schemes) {
  return file_system_registry_->GetRegisteredFileSystemSchemes(schemes);
}
  
Status Env::RegisterFileSystem(const string& scheme,
                               FileSystemRegistry::Factory factory) {
  return file_system_registry_->Register(scheme, factory);
}
  
Status Env::NewRandomAccessFile(const string& fname,
                                std::unique_ptr<RandomAccessFile>* result) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->NewRandomAccessFile(fname, result);
}
  
Status Env::NewReadOnlyMemoryRegionFromFile(
      const string& fname, std::unique_ptr<ReadOnlyMemoryRegion>* result) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->NewReadOnlyMemoryRegionFromFile(fname, result);
}

Status Env::NewWritableFile(const string& fname,
                            std::unique_ptr<WritableFile>* result) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->NewWritableFile(fname, result);
}

Status Env::NewAppendableFile(const string& fname,
                              std::unique_ptr<WritableFile>* result) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->NewAppendableFile(fname, result);
}
  
bool Env::FileExists(const string& fname) {
  FileSystem* fs;
  if (!GetFileSystemForFile(fname, &fs).ok()) {
    return false;
  }
  return fs->FileExists(fname);
}
  
bool Env::SetNonBlocking(int fd) {
  FileSystem* fs;
  if (!GetFileSystemForFile("", &fs).ok()) {
    return false;
  }
  return fs->SetNonBlocking(fd);
}

Status Env::GetChildren(const string& dir, std::vector<string>* result) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(dir, &fs));
  return fs->GetChildren(dir, result);
}
  
Status Env::GetMatchingPaths(const string& pattern,
                             std::vector<string>* results) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(pattern, &fs));
  return fs->GetMatchingPaths(pattern, results);
}
  
Status Env::DeleteFile(const string& fname) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->DeleteFile(fname);
}
  
Status Env::CreateDirectoryRecursively(const string& dirname) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(dirname, &fs));
  return fs->CreateDirectoryRecursively(dirname);
}
  
Status Env::CreateDirectory(const string& dirname) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(dirname, &fs));
  return fs->CreateDirectory(dirname);
}

Status Env::DeleteDirectory(const string& dirname) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(dirname, &fs));
  return fs->DeleteDirectory(dirname);
}
  
Status Env::Stat(const string& fname, FileStatistics* stat) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->Stat(fname, stat);
}
  
Status Env::IsDirectory(const string& fname) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->IsDirectory(fname);
}
  
Status Env::DeleteDirectoryRecursively(const string& dirname, int64* undeleted_files,
                                       int64* undeleted_dirs) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(dirname, &fs));
  return fs->DeleteDirectoryRecursively(dirname, undeleted_files, undeleted_dirs);
}
  
Status Env::GetFileSize(const string& fname, uint64* file_size) {
  FileSystem* fs;
  RETURN_IF_ERROR(GetFileSystemForFile(fname, &fs));
  return fs->GetFileSize(fname, file_size);
}
  
Status Env::RenameFile(const string& src, const string& target) {
  FileSystem* src_fs;
  FileSystem* target_fs;
  RETURN_IF_ERROR(GetFileSystemForFile(src, &src_fs));
  RETURN_IF_ERROR(GetFileSystemForFile(target, &target_fs));
  if (src_fs != target_fs) {
    return errors::Unimplemented("Renaming ", src, " to ", target,
                                 " not implemented");
  }
  return src_fs->RenameFile(src, target);
}
  
Thread::~Thread() {}

EnvDecorator::~EnvDecorator() {}

Status ReadFileToString(Env* env, const string& fname, string* data) {
  uint64 file_size;
  Status s = env->GetFileSize(fname, &file_size);
  if (!s.ok()) {
    return s;
  }
  std::unique_ptr<RandomAccessFile> file;
  s = env->NewRandomAccessFile(fname, &file);
  if (!s.ok()) {
    return s;
  }
  gtl::STLStringResizeUninitialized(data, file_size);
  char* p = gtl::string_as_array(data);
  StringPiece result;
  s = file->Read(0, file_size, &result, p);
  if (!s.ok()) {
    data->clear();
  } else if (result.size() != file_size) {
    s = errors::Aborted("File ", fname, " changed while reading: ", file_size,
                                                    " vs. ", result.size());
    data->clear();
  } else if (result.data() == p) {
      ; // Data is already in the correct location
  } else {
    memmove(p, result.data(), result.size());
  }
  return s;
}
  
Status WriteStringToFile(Env* env, const string& fname,
                         const StringPiece& data) {
  std::unique_ptr<WritableFile> file;
  Status s = env->NewWritableFile(fname, &file);
  if (!s.ok()) {
    return s;
  }
  s = file->Append(data);
  if (s.ok()) {
    s = file->Close();
  }
  return s;
}

namespace {

class FileStream : public google::protobuf::io::ZeroCopyInputStream {
 public:
  explicit FileStream(RandomAccessFile* file) : file_(file), pos_(0) {}
  
  void BackUp(int count) override { pos_ -= count; }
  bool Skip(int count) override {
    pos_ += count;
    return true;
  }
  int64_t ByteCount() const override { return pos_; }
  Status status() const { return status_; }
  
  bool Next(const void** data, int* size) override {
    StringPiece result;
    Status s = file_->Read(pos_, kBufSize, &result, scratch_);
    if (result.empty()) {
      status_ = s;
      return false;
    }
    pos_ += result.size();
    *data = result.data();
    *size = result.size();
    return true;
  }
  
 private:
  static const int kBufSize = 512 << 10;
  
  RandomAccessFile* file_;
  int64_t pos_;
  Status status_;
  char scratch_[kBufSize];
};

} // namespace
} // namespace base
