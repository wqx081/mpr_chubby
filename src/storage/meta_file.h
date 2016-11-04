#ifndef MPR_CHUBBY_STORAGE_META_FILE_H_
#define MPR_CHUBBY_STORAGE_META_FILE_H_
#include "storage/meta_file_interface.h"
#include "base/io/inputbuffer.h"

namespace mpr {
namespace chubby {

class MetaFile : public MetaFileInterface {
 public:
  MetaFile(const std::string& file_path); //, bool create_if_missing=true);
  ~MetaFile() {}

  virtual base::Status WriteLine(const std::string& line) override;
  virtual base::Status ReadLine(std::string* result) override;
  virtual base::Status ReadLastLine(std::string* result) override;

 private:
  const std::string file_path_;
  std::unique_ptr<base::io::InputBuffer> input_buffer_;
  std::unique_ptr<base::RandomAccessFile> input_file_;
  std::unique_ptr<base::WritableFile> output_file_;

  static const int kMaxInputBufferLength = 10;
};

} // namespace chubby
} // namespace mpr
#endif // MPR_CHUBBY_STORAGE_META_FILE_H_
