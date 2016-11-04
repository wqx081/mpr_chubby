#ifndef MPR_CHUBBY_STORAGE_META_FILE_INTERFACE_H_
#define MPR_CHUBBY_STORAGE_META_FILE_INTERFACE_H_

#include "base/macros.h"
#include "base/status.h"

namespace mpr {
namespace chubby {

class MetaFileInterface {
 public:
//  MetaFileInterface(const std::string& file_path_, bool create_if_missing=true);
  MetaFileInterface() {}
  virtual ~MetaFileInterface() {}

  virtual base::Status WriteLine(const std::string& line) = 0;
  virtual base::Status ReadLine(std::string* result) = 0;
  virtual base::Status ReadLastLine(std::string* result) = 0;
};

} // namespace chubby
} // namespace mpr
#endif // MPR_CHUBBY_STORAGE_META_FILE_INTERFACE_H_
