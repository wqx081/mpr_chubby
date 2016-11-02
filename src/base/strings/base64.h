#ifndef BASE_STRINGS_BASE64_H_
#define BASE_STRINGS_BASE64_H_

#include <stdlib.h>
#include <vector>
#include "base/stringpiece.h"

namespace base {
namespace strings {

void Base64Encode(StringPiece src, std::string* dest);
bool Base64Decode(StringPiece src, std::string* dest);

} // namespace strings
} // namespace base
#endif // BASE_STRINGS_BASE64_H_
