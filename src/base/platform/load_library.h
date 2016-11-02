#ifndef BASE_PLATFORM_LOAD_LIBRARY_H_
#define BASE_PLATFORM_LOAD_LIBRARY_H_

#include "base/status.h"

namespace base {

namespace internal {

Status LoadLibrary(const char* library_filename, void** handle);
Status GetSymbolFromLibrary(void* handle, const char* symbol_name,
                            void** symbol);
// Return the filename of a dynamically linked library formatted according to
// platform naming conventions
string FormatLibraryFileName(const string& name, const string& version);

}  // namespace internal
}  // namespace base
#endif  // BASE_PLATFORM_LOAD_LIBRARY_H_
