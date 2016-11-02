#include "base/platform/snappy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <snappy.h>

namespace base {
namespace port {

bool Snappy_Compress(const char* input, size_t length, string* output) {
    output->resize(snappy::MaxCompressedLength(length));
    size_t outlen;
    snappy::RawCompress(input, length, &(*output)[0], &outlen);
    output->resize(outlen);
    return true;
} 
    
bool Snappy_GetUncompressedLength(const char* input, size_t length,
                                  size_t* result) {
  return snappy::GetUncompressedLength(input, length, result);
} 
      
bool Snappy_Uncompress(const char* input, size_t length, char* output) {
  return snappy::RawUncompress(input, length, output);
}

} // namespace port
} // namespace base
