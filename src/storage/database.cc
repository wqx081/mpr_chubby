#include "storage/database.h"
#include "base/logging.h"
#include "base/platform/env.h"
#include "base/errors.h"
#include "base/io/path.h"
#include "base/gtl/map_util.h"

#include <gflags/gflags.h>

DECLARE_bool(chubby_data_compress);
DECLARE_int32(chubby_data_block_size);
DECLARE_int32(chubby_data_write_buffer_size);

namespace mpr {
namespace chubby {

const std::string Database::kAnonymousUser = "";

// Iterator
std::string Database::Iterator::key() const {
  return iterator_ ? iterator_->key().ToString() : "";
}

std::string Database::Iterator::value() const {
  return iterator_ ? iterator_->value().ToString() : "";
}

Database::Iterator* Database::Iterator::Seek(const std::string& key) {
  if (iterator_) {
    iterator_->Seek(key);
  }
  return this;
}

Database::Iterator* Database::Iterator::Next() {
  if (iterator_) {
    iterator_->Next();
  }
  return this;
}

bool Database::Iterator::Valid() const {
  return iterator_ ? iterator_->Valid() : false;
}

base::Status Database::Iterator::status() const {
  if (iterator_) {
    if (iterator_->status().ok()) {
      return base::Status::OK();
    }
    return base::errors::Internal(iterator_->status().ToString());
  }  
  return base::errors::Internal("Not Open leveldb");
}

//
Database::Database(const std::string& db_path) : db_path_(db_path) {

  base::Status status = base::Env::Default()->CreateDirectory(db_path_);
  DCHECK(status.ok() || status.code() == base::error::ALREADY_EXISTS) << status.ToString(); 
  std::unique_ptr<leveldb::DB> db;
  DoOpenDB(&db);
  db_map_[""] = std::move(db);  
}

Database::~Database() {
  base::mutex_lock l(mu_);
  for (auto& kv : db_map_) {
    kv.second.reset(nullptr);
  }
}

bool Database::Open(const std::string& name) {
  base::mutex_lock l(mu_);
  if (db_map_.find(name) != db_map_.end()) {
    return true;
  }
  std::unique_ptr<leveldb::DB> db;
  DoOpenDB(&db, name);
  db_map_[name] = std::move(db);
  return true;
}

void Database::Close(const std::string& name) {
  base::mutex_lock l(mu_);
  auto it = db_map_.find(name);
  if (it == db_map_.end()) {
    return;
  }
  it->second.reset(nullptr);
  db_map_.erase(it);
}

base::Status Database::Get(const std::string& name,
                           const std::string& key,
                           std::string* value) {
  if (value == nullptr) {
    return base::errors::InvalidArgument("value == nullptr");
  }
  base::mutex_lock l(mu_);
  auto it = db_map_.find(name);
  if (it == db_map_.end()) {
    LOG(WARNING) << "[GET] Not existed: " << name;
    return base::errors::NotFound("Not found db name: ", name);
  }
  leveldb::Status status = it->second->Get(leveldb::ReadOptions(), key, value);
  if (status.ok()) {
    return base::Status::OK();
  } else if (status.IsNotFound()) {
    return base::errors::NotFound("Not found key: ", key, " at name: ", name);
  }
  return base::errors::Internal("leveldb: " + status.ToString());
}

base::Status Database::Put(const std::string& name,
                           const std::string& key,
                           const std::string& value) {
  base::mutex_lock l(mu_);
  auto it = db_map_.find(name);
  if (it == db_map_.end()) {
    LOG(WARNING) << "[PUT] Not existed: " << name;
    return base::errors::NotFound("Not found db name: ", name);
  }
  leveldb::Status status = it->second->Put(leveldb::WriteOptions(), key, value);
  if (status.ok()) {
    return base::Status::OK();
  }
  return base::errors::Internal("leveldb: " + status.ToString());
}

base::Status Database::Delete(const std::string& name,
                              const std::string& key) {
  base::mutex_lock l(mu_);
  auto it = db_map_.find(name);
  if (it == db_map_.end()) {
    LOG(WARNING) << "[DELETE] Not existed: " << name;
    return base::errors::NotFound("Not found db name: ", name);
  }
  leveldb::Status status = it->second->Delete(leveldb::WriteOptions(), key);
  if (status.ok()) {
    return base::Status::OK();
  }
  return base::errors::Internal("leveldb: " + status.ToString());
}

Database::Iterator* Database::NewIterator(const std::string& name) {
  base::mutex_lock l(mu_);
  auto it = db_map_.find(name);
  if (it == db_map_.end()) {
    LOG(WARNING) << "Not existed: " << name;
    return nullptr;
  }
  return new Database::Iterator(it->second.get(), leveldb::ReadOptions());
}

void Database::DoOpenDB(std::unique_ptr<leveldb::DB>* result, const std::string& name) {
  std::string full_name = base::io::JoinPath(db_path_, name + std::string("@db"));        
  leveldb::Options options;                                           
  options.create_if_missing = true;                                   
  if (FLAGS_chubby_data_compress) {                                   
    options.compression = leveldb::kSnappyCompression;                
    LOG(INFO) << "Enable snappy compress for data storage";           
  }                                                                   
  options.write_buffer_size = FLAGS_chubby_data_write_buffer_size * 1024 * 1024;
  options.block_size = FLAGS_chubby_data_block_size * 1024;           
  LOG(INFO) << "[data]: block_size: " << options.block_size << ", writer_buffer_size: " << options.write_buffer_size;
  leveldb::DB* db = nullptr;                                          
  leveldb::Status status = leveldb::DB::Open(options, full_name, &db);
  DCHECK(status.ok());         
  result->reset(db);
}


} // namespace chubby
} // namespace mpr
