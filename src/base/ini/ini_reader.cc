#include "base/ini/ini_reader.h"
#include "base/ini/ini.h"

#include "base/strings/numbers.h"
#include "base/strings/str_util.h"
#include "base/strings/strcat.h"

#include "base/gtl/map_util.h"

#include <algorithm>
#include <utility>

namespace base {

const std::string INIReader::kDefaultString = "";
const double INIReader::kDefaultFloat = 0.0;
const bool INIReader::kDefaultBoolean = false;

INIReader::INIReader(const std::string& file_name) {
  error_ = ini_parse(file_name.c_str(), ValueHandler, this);
}

INIReader::~INIReader() {
  std::map<std::string, std::set<std::string>*>::iterator it;
  for (it = fields_.begin(); it != fields_.end(); ++it) {
    delete it->second;
  }
}

int INIReader::error() const {
  return error_;
}

std::string INIReader::GetString(const std::string& section,
                                 const std::string& name,
                                 const std::string& default_value) {
  std::string key = MakeKey(section, name);
  return values_.count(key) ? values_[key] : default_value;
}

int32_t INIReader::GetInteger(const std::string& section,
                              const std::string& name,
                              int32_t default_value) {
  std::string str_value = GetString(section, name, kDefaultString);  
  int32 v;
  if (!strings::safe_strto32(str_value, &v)) {
    return default_value;
  }
  return v;
}

double INIReader::GetFloat(const std::string& section,
                           const std::string& name,
                           double default_value) {
  std::string str_value = GetString(section, name, kDefaultString);
  double v;
  if (!strings::safe_strtod(str_value.c_str(), &v)) {
    return default_value;
  }
  return v;
}

bool INIReader::GetBoolean(const std::string& section,
                           const std::string& name,
                           bool default_value) {
  std::string str_value = strings::Lowercase(GetString(section, name, kDefaultString));  
  if (str_value == "true" ||
      str_value == "yes"  ||
      str_value == "on"   ||
      str_value == "1") {
    return true;
  }

  if (str_value == "false" ||
      str_value == "no"    ||
      str_value == "off"   ||
      str_value == "0") {
    return false;
  }

  return default_value;
}

std::set<std::string> INIReader::GetSections() const {
  return sections_;
}

std::set<std::string> INIReader::GetFields(const std::string& section) const {
  std::string section_key = strings::Lowercase(section);
  std::set<std::string>* r = gtl::FindPtrOrNull(fields_, section_key);
  if (!r) {
    return std::set<std::string>();
  }
  return *r;
}

void INIReader::Set(const std::string& section, const std::string& name,
                    const std::string& value) {
  std::string key = MakeKey(section, name);
  values_[key] = "";
  ValueHandler(this, section.c_str(), name.c_str(), value.c_str());
}

// static
std::string INIReader::MakeKey(const std::string& section,
                               const std::string& name) {
  std::string key = strings::Lowercase(strings::StrCat(section, "=", name));
  return key;
}

// static
int INIReader::ValueHandler(void* user,
                            const char* section,
                            const char* name,
                            const char* value) {
  INIReader* reader = reinterpret_cast<INIReader*>(user);
  std::string key = MakeKey(section, name);
  if (reader->values_[key].size() > 0)
    reader->values_[key] += "\n";
  reader->values_[key] += value;

  reader->sections_.insert(section);
  
  std::string section_key = strings::Lowercase(section);
  
  std::set<std::string>* field_set;

  field_set = gtl::FindPtrOrNull(reader->fields_, section_key);
  if (!field_set) {
    field_set = gtl::LookupOrInsert(&reader->fields_, section_key, new std::set<std::string>());
  }
  field_set->insert(name);

  return 1;
}


} // namespace base
