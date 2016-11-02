#include "base/platform/load_library.h"
#include "base/errors.h"

#include <dlfcn.h>


namespace base {
namespace internal {

Status LoadLibrary(const char* library_filename, void** handle) {
  *handle = dlopen(library_filename, RTLD_NOW | RTLD_LOCAL);
  if (!*handle) {
    return errors::NotFound(dlerror());
  }
  return Status::OK();
}

Status GetSymbolFromLibrary(void* handle, const char* symbol_name,
                            void** symbol) {
  *symbol = dlsym(handle, symbol_name);
  if (!*symbol) {
    return errors::NotFound(dlerror());
  }
  return Status::OK();
}

string FormatLibraryFileName(const string& name, const string& version) {
  string filename;
  if (version.size() == 0) {
    filename = "lib" + name + ".so";
  } else {
    filename = "lib" + name + ".so" + "." + version;
  }
  return filename;
}

}  // namespace internal
}  // namespace base
