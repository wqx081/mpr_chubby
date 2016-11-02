#include "base/monitoring/util/protobuf.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
  
namespace base {
namespace monitoring {

using google::protobuf::MessageLite;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::io::OstreamOutputStream;
using google::protobuf::io::ZeroCopyOutputStream;
  
  
bool WriteDelimitedTo(const MessageLite& message,
                      ZeroCopyOutputStream* rawOutput) {
  CodedOutputStream output(rawOutput);
    
  const int size = message.ByteSize();
  output.WriteVarint32(size);
    
  uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != NULL) {
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    message.SerializeWithCachedSizes(&output);
    if (output.HadError())
      return false;
  }   
    
  return true;
} 
  
  
bool WriteDelimitedToOstream(const MessageLite& message, std::ostream* os) {
  OstreamOutputStream oos(os);
  return WriteDelimitedTo(message, &oos);
} 

} // namespace monitoring
}  // namespace base
