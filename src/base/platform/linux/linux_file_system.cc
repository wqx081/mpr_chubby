#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "base/errors.h"
#include "base/status.h"
#include "base/strings/strcat.h"
#include "base/platform/env.h"
#include "base/logging.h"
#include "base/platform/linux/error.h"
#include "base/platform/linux/linux_file_system.h"

namespace base {

// pread() based random-access
class LinuxRandomAccessFile : public RandomAccessFile {
 private:
  string filename_;
  int fd_;

 public:
  LinuxRandomAccessFile(const string& fname, int fd)
      : filename_(fname), fd_(fd) {}
  ~LinuxRandomAccessFile() override { close(fd_); }

  Status Read(uint64 offset, size_t n, StringPiece* result,
              char* scratch) const override {
    Status s;
    char* dst = scratch;
    while (n > 0 && s.ok()) {
      ssize_t r = pread(fd_, dst, n, static_cast<off_t>(offset));
      if (r > 0) {
        dst += r;
        n -= r;
        offset += r;
      } else if (r == 0) {
        s = Status(error::OUT_OF_RANGE, "Read less bytes than requested");
      } else if (errno == EINTR || errno == EAGAIN) {
        // Retry
      } else {
        s = IOError(filename_, errno);
      }
    }
    *result = StringPiece(scratch, dst - scratch);
    return s;
  }
};

class LinuxWritableFile : public WritableFile {
 private:
  string filename_;
  FILE* file_;

 public:
  LinuxWritableFile(const string& fname, FILE* f)
      : filename_(fname), file_(f) {}

  ~LinuxWritableFile() override {
    if (file_ != NULL) {
      // Ignoring any potential errors
      fclose(file_);
    }
  }

  Status Append(const StringPiece& data) override {
    size_t r = fwrite(data.data(), 1, data.size(), file_);
    if (r != data.size()) {
      return IOError(filename_, errno);
    }
    return Status::OK();
  }

  Status Close() override {
    Status result;
    if (fclose(file_) != 0) {
      result = IOError(filename_, errno);
    }
    file_ = NULL;
    return result;
  }

  Status Flush() override {
    if (fflush(file_) != 0) {
      return IOError(filename_, errno);
    }
    return Status::OK();
  }

  Status Sync() override {
    Status s;
    if (fflush(file_) != 0) {
      s = IOError(filename_, errno);
    }
    return s;
  }
};

class LinuxReadOnlyMemoryRegion : public ReadOnlyMemoryRegion {
 public:
  LinuxReadOnlyMemoryRegion(const void* address, uint64 length)
      : address_(address), length_(length) {}
  ~LinuxReadOnlyMemoryRegion() { munmap(const_cast<void*>(address_), length_); }
  const void* data() override { return address_; }
  uint64 length() override { return length_; }

 private:
  const void* const address_;
  const uint64 length_;
};

Status LinuxFileSystem::NewRandomAccessFile(
    const string& fname, std::unique_ptr<RandomAccessFile>* result) {
  string translated_fname = TranslateName(fname);
  Status s;
  int fd = open(translated_fname.c_str(), O_RDONLY);
  if (fd < 0) {
    s = IOError(fname, errno);
  } else {
    result->reset(new LinuxRandomAccessFile(translated_fname, fd));
  }
  return s;
}

Status LinuxFileSystem::NewWritableFile(const string& fname,
                                        std::unique_ptr<WritableFile>* result) {
  string translated_fname = TranslateName(fname);
  Status s;
  FILE* f = fopen(translated_fname.c_str(), "w");
  if (f == NULL) {
    s = IOError(fname, errno);
  } else {
    result->reset(new LinuxWritableFile(translated_fname, f));
  }
  return s;
}

Status LinuxFileSystem::NewAppendableFile(
    const string& fname, std::unique_ptr<WritableFile>* result) {
  string translated_fname = TranslateName(fname);
  Status s;
  FILE* f = fopen(translated_fname.c_str(), "a");
  if (f == NULL) {
    s = IOError(fname, errno);
  } else {
    result->reset(new LinuxWritableFile(translated_fname, f));
  }
  return s;
}

Status LinuxFileSystem::NewReadOnlyMemoryRegionFromFile(
    const string& fname, std::unique_ptr<ReadOnlyMemoryRegion>* result) {
  string translated_fname = TranslateName(fname);
  Status s = Status::OK();
  int fd = open(translated_fname.c_str(), O_RDONLY);
  if (fd < 0) {
    s = IOError(fname, errno);
  } else {
    struct stat st;
    ::fstat(fd, &st);
    const void* address =
        mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (address == MAP_FAILED) {
      s = IOError(fname, errno);
    } else {
      result->reset(new LinuxReadOnlyMemoryRegion(address, st.st_size));
    }
    close(fd);
  }
  return s;
}

bool LinuxFileSystem::FileExists(const string& fname) {
  return access(TranslateName(fname).c_str(), F_OK) == 0;
}

bool LinuxFileSystem::SetNonBlocking(int fd) {
  const int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    return false;
  if (flags & O_NONBLOCK)
    return true;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1;
}

Status LinuxFileSystem::GetChildren(const string& dir,
                                    std::vector<string>* result) {
  string translated_dir = TranslateName(dir);
  result->clear();
  DIR* d = opendir(translated_dir.c_str());
  if (d == NULL) {
    return IOError(dir, errno);
  }
  struct dirent* entry;
  while ((entry = readdir(d)) != NULL) {
    StringPiece basename = entry->d_name;
    if ((basename != ".") && (basename != "..")) {
      result->push_back(entry->d_name);
    }
  }
  closedir(d);
  return Status::OK();
}

Status LinuxFileSystem::DeleteFile(const string& fname) {
  Status result;
  if (unlink(TranslateName(fname).c_str()) != 0) {
    result = IOError(fname, errno);
  }
  return result;
}

Status LinuxFileSystem::CreateDirectory(const string& name) {
  Status result;
  if (mkdir(TranslateName(name).c_str(), 0755) != 0) {
    result = IOError(name, errno);
  }
  return result;
}

Status LinuxFileSystem::DeleteDirectory(const string& name) {
  Status result;
  if (rmdir(TranslateName(name).c_str()) != 0) {
    result = IOError(name, errno);
  }
  return result;
}

Status LinuxFileSystem::GetFileSize(const string& fname, uint64* size) {
  Status s;
  struct stat sbuf;
  if (stat(TranslateName(fname).c_str(), &sbuf) != 0) {
    *size = 0;
    s = IOError(fname, errno);
  } else {
    *size = sbuf.st_size;
  }
  return s;
}

Status LinuxFileSystem::Stat(const string& fname, FileStatistics* stats) {
  Status s;
  struct stat sbuf;
  if (stat(TranslateName(fname).c_str(), &sbuf) != 0) {
    s = IOError(fname, errno);
  } else {
    stats->length = sbuf.st_size;
    stats->mtime_nsec = sbuf.st_mtime * 1e9;
    stats->is_directory = S_ISDIR(sbuf.st_mode);
  }
  return s;
}

Status LinuxFileSystem::RenameFile(const string& src, const string& target) {
  Status result;
  if (rename(TranslateName(src).c_str(), TranslateName(target).c_str()) != 0) {
    result = IOError(src, errno);
  }
  return result;
}

}  // namespace base
