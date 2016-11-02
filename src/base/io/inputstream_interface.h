#ifndef BASE_IO_INPUTSTREAM_INTERFACE_H_
#define BASE_IO_INPUTSTREAM_INTERFACE_H_

#include <string>
#include "base/status.h"
#include "base/port.h"


namespace base {
namespace io {

// An interface that defines input streaming operations.
class InputStreamInterface {
 public:
  InputStreamInterface() {}
  virtual ~InputStreamInterface() {}

  // Reads the next bytes_to_read from the file. Typical return codes:
  //  * OK - in case of success.
  //  * OUT_OF_RANGE - not enough bytes remaining before end of file.
  virtual Status ReadNBytes(int64 bytes_to_read, string* result) = 0;

  // Skips bytes_to_skip before next ReadNBytes. bytes_to_skip should be >= 0.
  // Typical return codes:
  //  * OK - in case of success.
  //  * OUT_OF_RANGE - not enough bytes remaining before end of file.
  virtual Status SkipNBytes(int64 bytes_to_skip);

  // Return the offset of the current byte relative to the beginning of the
  // file.
  // If we Skip / Read beyond the end of the file, this should return the length
  // of the file.
  // If there are any errors, this must return -1.
  virtual int64 Tell() const = 0;

  // Resets the stream to the beginning.
  virtual Status Reset() = 0;
};

}  // namespace io
}  // namespace base
#endif  // BASE_IO_INPUTSTREAM_INTERFACE_H_
