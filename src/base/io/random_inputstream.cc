#include "base/io/random_inputstream.h"
#include <memory>

namespace base {
namespace io {

RandomAccessInputStream::RandomAccessInputStream(RandomAccessFile* file,
                                                 bool owns_file)
    : file_(file), owns_file_(owns_file) {}

RandomAccessInputStream::~RandomAccessInputStream() {
  if (owns_file_) {
    delete file_;
  }
}

Status RandomAccessInputStream::ReadNBytes(int64 bytes_to_read,
                                           string* result) {
  if (bytes_to_read < 0) {
    return errors::InvalidArgument("Cannot read negative number of bytes");
  }
  result->clear();
  result->resize(bytes_to_read);
  char* result_buffer = &(*result)[0];
  StringPiece data;
  Status s = file_->Read(pos_, bytes_to_read, &data, result_buffer);
  if (data.data() != result_buffer) {
    memmove(result_buffer, data.data(), data.size());
  }
  result->resize(data.size());
  if (s.ok() || errors::IsOutOfRange(s)) {
    pos_ += data.size();
  } else {
    return s;
  }
  // If the amount of data we read is less than what we wanted, we return an
  // out of range error. We need to catch this explicitly since file_->Read()
  // would not do so if at least 1 byte is read (b/30839063).
  if ((int64)data.size() < bytes_to_read) {
    return errors::OutOfRange("reached end of file");
  }
  return Status::OK();
}

int64 RandomAccessInputStream::Tell() const { return pos_; }

}  // namespace io
}  // namespace base
