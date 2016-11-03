#include "storage/bin_logger.h"

#include "base/platform/env.h"
#include "base/io/path.h"
#include "base/string_encode.h"
#include "base/logging.h"

#include <leveldb/write_batch.h>

namespace {

const std::string kLogDbName = "#binlog";
const std::string kLengthTag = "#BINLOG_LENGTH#";

} // namespace

namespace mpr {
namespace chubby {

BinLogger::BinLogger(const Options& options)
    : length_(0), last_log_term_(-1) {

  base::Status status = base::Env::Default()->CreateDirectory(kLogDbName);
  DCHECK(status.ok() || status.code() == base::error::ALREADY_EXISTS) << status.ToString(); 

  std::string full_path = base::io::JoinPath(options.db_path, kLogDbName);
  leveldb::Options db_options;
  db_options.create_if_missing = true;
  if (options.compress) {
    db_options.compression = leveldb::kSnappyCompression;
  }
  db_options.write_buffer_size = options.write_buffer_size;
  db_options.block_size = options.block_size;

  // Create leveldb
  leveldb::DB* db;
  leveldb::Status db_status = leveldb::DB::Open(db_options, full_path, &db);
  if (!db_status.ok()) {
  }
  db_.reset(db);

  // Read LastLogTerm
  std::string value;
  db_status = db_->Get(leveldb::ReadOptions(), kLengthTag, &value);
  if (db_status.ok() && !value.empty()) {
    //DCHECK(base::strings::safe_strto64(value, (base::int64 *)&length_)) << "value: " << value;
    length_ = KeyToIndex(value);
    LOG(INFO) << "Length: " << length_;
    if (length_ > 0) {
      LogEntry log_entry;
      DCHECK(ReadSlot(length_ - 1, &log_entry));
      last_log_term_ = log_entry.term;
    }
  }
}

BinLogger::~BinLogger() {}
      
int64_t BinLogger::GetLength() const {
  base::mutex_lock l(mu_);
  return length_;
}

bool BinLogger::ReadSlot(int64_t slot_index, LogEntry* result) {
  std::string value;
  std::string key = IndexToKey(slot_index);
  leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, &value);
  if (status.ok()) {
    StringToLogEntry(value, result);
    return true;
  } else if (status.IsNotFound()) {
    return false;
  } else {
    DCHECK(false);
  }
  return false;
}


bool BinLogger::RemoveSlot(int64_t slot_index) {
  std::string value;
  std::string key = IndexToKey(slot_index); 
  leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, &value);
  if (!status.ok()) {
    return false;
  }
  status = db_->Delete(leveldb::WriteOptions(), key);
  if (status.ok()) {
    return true;
  } else {
    return false;
  }
}

bool BinLogger::RemoveSlotBefore(int64_t slot_gc_index) {
  (void) slot_gc_index;
  // TODO(wqx):
  // db_->SetNexusGCKey(slot_gc_index);
  // DBImpl::CompactRange
  db_->CompactRange(nullptr, nullptr);
  return true;
}

void BinLogger::AppendEntryList(const google::protobuf::RepeatedPtrField<mpr::chubby::Entry>& entries) {
  base::mutex_lock l(mu_);
  leveldb::WriteBatch batch;
  int64_t current_index = length_;
  std::string next_index = IndexToKey(length_ + entries.size());
  for (int i = 0; i < entries.size(); i++) {
    LogEntry log_entry;
    std::string buf;
    log_entry.log_operation = entries.Get(i).op();
    log_entry.user = entries.Get(i).user();
    log_entry.key = entries.Get(i).key();
    log_entry.value = entries.Get(i).value();
    log_entry.term = entries.Get(i).term();
    last_log_term_ =  log_entry.term;
    LogEntryToString(log_entry, &buf);

    batch.Put(IndexToKey(current_index + i), buf);
  }
  batch.Put(kLengthTag, next_index);
  leveldb::Status status = db_->Write(leveldb::WriteOptions(), &batch);
  DCHECK(status.ok());
  length_ += entries.size();
}

void BinLogger::AppendEntry(const LogEntry& log_entry) {

  std::string buf;
  LogEntryToString(log_entry, &buf);
  std::string current_index;
  std::string next_index;

  base::mutex_lock l(mu_);

  current_index = IndexToKey(length_);
  next_index = IndexToKey(length_ + 1);
  leveldb::WriteBatch batch;
  batch.Put(current_index, buf);
  batch.Put(kLengthTag, next_index);
  leveldb::Status status = db_->Write(leveldb::WriteOptions(), &batch);

  DCHECK(status.ok());

  length_++;
  last_log_term_ = log_entry.term;
}

void BinLogger::Truncate(int64_t trunk_slot_index) {
  if (trunk_slot_index < -1)
    trunk_slot_index = -1;
  
  base::mutex_lock l(mu_);
  length_ = trunk_slot_index + 1;
  leveldb::Status status = db_->Put(leveldb::WriteOptions(),
                                    kLengthTag, IndexToKey(length_));
  DCHECK(status.ok());
  if (length_ > 0) {
    LogEntry log_entry;
    DCHECK(ReadSlot(length_ - 1, &log_entry));
    last_log_term_ = log_entry.term;
  }
}

void BinLogger::GetLastLogIndexAndTerm(int64_t* log_index, int64_t* log_term) const {
  base::mutex_lock l(mu_);
  *log_index = length_ - 1;
  *log_term = last_log_term_;
}

void BinLogger::LogEntryToString(const LogEntry& log_entry, std::string* buf) {
  DCHECK(buf != nullptr);
  int32_t total_len = sizeof(uint8_t)
                          + sizeof(int32_t) + log_entry.user.size()
                          + sizeof(int32_t) + log_entry.key.size()
                          + sizeof(int32_t) + log_entry.value.size()
                          + sizeof(int64_t);
  buf->resize(total_len);
  int32_t user_size = log_entry.user.size();
  int32_t key_size = log_entry.key.size();
  int32_t value_size = log_entry.value.size();
  char* p = reinterpret_cast<char*>(& ((*buf)[0]));
  p[0] = static_cast<uint8_t>(log_entry.log_operation);
  p += sizeof(uint8_t);
  memcpy(p, static_cast<const void*>(&user_size), sizeof(int32_t));
  p += sizeof(int32_t);
  memcpy(p, static_cast<const void*>(log_entry.user.data()), log_entry.user.size());
  p += log_entry.user.size();
  memcpy(p, static_cast<const void*>(&key_size), sizeof(int32_t));
  p += sizeof(int32_t);
  memcpy(p, static_cast<const void*>(log_entry.key.data()), log_entry.key.size());
  p += log_entry.key.size();
  memcpy(p, static_cast<const void*>(&value_size), sizeof(int32_t));
  p += sizeof(int32_t);
  memcpy(p, static_cast<const void*>(log_entry.value.data()), log_entry.value.size());
  p += log_entry.value.size();
  memcpy(p, static_cast<const void*>(&log_entry.term), sizeof(int64_t));
}

void BinLogger::StringToLogEntry(const std::string& buf, LogEntry* log_entry) {
  DCHECK(log_entry != nullptr);
  const char* p = buf.data();
  int32_t user_size = 0;
  int32_t key_size = 0;
  int32_t value_size = 0;
  uint8_t opcode = 0;
  memcpy(static_cast<void*>(&opcode), p, sizeof(uint8_t));
  log_entry->log_operation= static_cast<LogOperation>(opcode);
  p += sizeof(uint8_t);
  memcpy(static_cast<void*>(&user_size), p, sizeof(int32_t));
  log_entry->user.resize(user_size);
  p += sizeof(int32_t);
  memcpy(static_cast<void*>(&log_entry->user[0]), p, user_size);
  p += user_size;
  memcpy(static_cast<void*>(&key_size), p, sizeof(int32_t));
  log_entry->key.resize(key_size);
  p += sizeof(int32_t);
  memcpy(static_cast<void*>(&log_entry->key[0]), p, key_size);
  p += key_size;
  memcpy(static_cast<void*>(&value_size), p, sizeof(int32_t));
  log_entry->value.resize(value_size);
  p += sizeof(int32_t);
  memcpy(static_cast<void*>(&log_entry->value[0]), p, value_size);
  p += value_size;
  memcpy(static_cast<void*>(&log_entry->term), p , sizeof(int64_t));
}

// static
std::string BinLogger::IndexToKey(int64_t index) {
  const char nibble[] = "0123456789abcdef";
  std::string index_str(sizeof(index) * 2, nibble[0]);
  for (int i = sizeof(index) * 2; i > 0 && index > 0; --i) {
    index_str[i - 1] = nibble[index & 0xf];
    index = index >> 4;
  }

  return index_str;
}

// static
int64_t BinLogger::KeyToIndex(const std::string& key) {
  const std::string index_str(base::HexDecode(key));

  int64_t index(0);
  CHECK_EQ(index_str.size(), sizeof(index));
  for (size_t i = 0; i < sizeof(index); ++i) {
    index = (index << 8) | static_cast<unsigned char>(index_str[i]);
  }
  return index;
}

} // namespace chubby
} // namespace mpr
