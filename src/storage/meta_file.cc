#include "storage/meta_file.h"
#include "base/port.h"

namespace mpr {
namespace chubby {

MetaFile::MetaFile(const std::string& file_path)
    : file_path_(file_path) {

  if (base::Env::Default()->FileExists(file_path)) {
    return;
  }
  // Create File
  DCHECK(base::WriteStringToFile(base::Env::Default(), file_path_, "").ok());
  DCHECK(base::Env::Default()->NewRandomAccessFile(file_path_, &input_file_).ok());
  DCHECK(base::Env::Default()->NewAppendableFile(file_path_, &output_file_).ok());
  auto in = new base::io::InputBuffer(input_file_.get(), kMaxInputBufferLength);
  DCHECK(in);
  input_buffer_.reset(in);
}

base::Status MetaFile::WriteLine(const std::string& line) {
  return output_file_->Append(line);
}

base::Status MetaFile::ReadLine(std::string* result) {
  return input_buffer_->ReadLine(result);
}

base::Status MetaFile::ReadLastLine(std::string* result) {
  
  base::int64 current_offset = input_buffer_->Tell();
  std::string line;
  base::Status status;
  while (true) {
      status = input_buffer_->ReadLine(&line);
      if (!status.ok()) {
        break;
      }
  }
  if (status.code() != base::error::OUT_OF_RANGE) {
    return status;
  }

  result->resize(line.size());
  result->swap(line);
  input_buffer_->Seek(current_offset);
  return status;
}

} // namespace chubby
} // namespace mpr
