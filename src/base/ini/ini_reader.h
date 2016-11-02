#ifndef BASE_INI_INI_READER_H_
#define BASE_INI_INI_READER_H_

#include <stdint.h>
#include <map>
#include <set>
#include <string>

namespace base {

class INIReader {
 public:
  explicit INIReader(const std::string& file_path);
  ~INIReader();

  int error() const;
  std::string GetString(const std::string& section,
                        const std::string& name,
                        const std::string& default_value=kDefaultString);
  int32_t GetInteger(const std::string& section,
                     const std::string& name,
                     int32_t default_value=kDefaultInteger);
  double GetFloat(const std::string& section,
                  const std::string& name,
                  double default_value=kDefaultFloat);
  bool GetBoolean(const std::string& section,
                  const std::string& name,
                  bool default_value=kDefaultBoolean);

  std::set<std::string> GetSections() const;
  std::set<std::string> GetFields(const std::string& section) const;

  void Set(const std::string& section,
           const std::string& name,
           const std::string& value);

 private:
  static const std::string kDefaultString;
  static const int kDefaultInteger = 0;
  static const double kDefaultFloat;
  static const bool kDefaultBoolean;

  int error_;
  std::map<std::string, std::string> values_;
  std::set<std::string> sections_;
  std::map<std::string, std::set<std::string>*> fields_;

  static std::string MakeKey(const std::string& section, const std::string& name);
  static int ValueHandler(void* user,
                          const char* section,
                          const char* name,
                          const char* value);
};

} // namespace base
#endif // BASE_INI_INI_READER_H_
