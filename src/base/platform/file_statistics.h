#ifndef BASE_PLATFORM_FILE_STATISTICS_H_
#define BASE_PLATFORM_FILE_STATISTICS_H_
#include "base/port.h"

namespace base {

struct FileStatistics {
  int64 length = -1;
  int64 mtime_nsec = 0;
  bool is_directory = false;
  
  FileStatistics() {}
  ~FileStatistics() {}
};

} // namespace base

#endif // BASE_PLATFORM_FILE_STATISTICS_H_
