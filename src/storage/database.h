#ifndef MPR_CHUBBY_STORAGE_DATABASE_H_
#define MPR_CHUBBY_STORAGE_DATABASE_H_

#include <unordered_map>
#include <functional>
#include <memory>

#include <leveldb/db.h>

#include "proto/service.pb.h"
#include "base/status.h"
#include "base/platform/mutex.h"

namespace mpr {
namespace chubby {

class Database {
 public:
  Database(const std::string& db_path);
  ~Database();

  bool Open(const std::string& name);
  void Close(const std::string& name);

  base::Status Get(const std::string& name, const std::string& key, std::string* value);
  base::Status Put(const std::string& name, const std::string& key, const std::string& value);
  base::Status Delete(const std::string& name, const std::string& key);

  static const std::string kAnonymousUser;
 public:

  class Iterator {
   public:
    Iterator() : iterator_(nullptr) {}
    Iterator(leveldb::DB* db, const leveldb::ReadOptions& options)
      : iterator_(db->NewIterator(options)) {}

    ~Iterator() {}
    
    std::string key() const;
    std::string value() const;

    Iterator* Seek(const std::string& key);
    Iterator* Next();

    bool Valid() const;
    base::Status status() const;

   private:
    std::unique_ptr<leveldb::Iterator> iterator_;
    DISALLOW_COPY_AND_ASSIGN(Iterator);
  };

  Iterator* NewIterator(const std::string& name);

 private:
  base::mutex mu_;
  std::string db_path_;
  std::unordered_map<std::string, std::unique_ptr<leveldb::DB>> db_map_;

  void DoOpenDB(std::unique_ptr<leveldb::DB>* result, const std::string& name="");

  DISALLOW_COPY_AND_ASSIGN(Database);
};

} // namespace chubby
} // namespace mpr
#endif // MPR_CHUBBY_STORAGE_DATABASE_H_
