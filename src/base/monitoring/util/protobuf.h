#ifndef BASE_MONITORING_UTIL_PROTOBUF_H_
#define BASE_MONITORING_UTIL_PROTOBUF_H_

#include <iostream>

namespace google {
namespace protobuf {
namespace io {
class ZeroCopyOutputStream;
}  // namespace io

class MessageLite;
}  // namespace protobuf
}  // namespace google

namespace base {
namespace monitoring {

bool WriteDelimitedTo(const google::protobuf::MessageLite& message,
                      google::protobuf::io::ZeroCopyOutputStream* rawOutput);
                      
bool WriteDelimitedToOstream(const google::protobuf::MessageLite& message,
                             std::ostream* os);
                             
} // namespace monitoring                             
} // namespace base

#endif // BASE_MONITORING_UTIL_PROTOBUF_H_
