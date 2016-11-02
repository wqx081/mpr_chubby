#include "base/io/inputstream_interface.h"
#include "base/errors.h"

namespace base {
namespace io {

// To limit memory usage, the default implementation of SkipNBytes() only reads
// 8MB at a time.
static constexpr int64 kMaxSkipSize = 8 * 1024 * 1024;

Status InputStreamInterface::SkipNBytes(int64 bytes_to_skip) {
  if (bytes_to_skip < 0) {
    return errors::InvalidArgument("Can't skip a negative number of bytes");
  }
  string unused;
  // Read kDefaultSkipSize at a time till bytes_to_skip.
  while (bytes_to_skip > 0) {
    int64 bytes_to_read = std::min<int64>(kMaxSkipSize, bytes_to_skip);
    RETURN_IF_ERROR(ReadNBytes(bytes_to_read, &unused));
    bytes_to_skip -= bytes_to_read;
  }
  return Status::OK();
}

}  // namespace io
}  // namespace base
