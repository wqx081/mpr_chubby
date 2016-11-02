#ifndef MPR_CHUBBY_STORAGE_BIN_LOGGER_H_
#define MPR_CHUBBY_STORAGE_BIN_LOGGER_H_

#include "base/macros.h"

#include <functional>
#include <memory>

#include "proto/service.pb.h"
#include "base/platform/mutex.h"

#include <leveldb/db.h>


namespace mpr {
namespace chubby {

struct LogEntry {
    LogOperation log_operation;
    std::string user;
    std::string key;
    std::string value;
    int64_t term;
    LogEntry() : log_operation(kNop), user("") {}
};

class BinLogger {
 public:

  struct Options {
    std::string db_path;
    bool compress;
    int32_t block_size;
    int32_t write_buffer_size;
    
    static const int32_t kDefaultBlockSize = 32748;
    static const int32_t kDefaultWriteBufferSize = 33554432;
    static const bool kDefaultCompress = false;

    Options(const std::string& db, bool c = kDefaultCompress, 
            int32_t bs = kDefaultBlockSize, int32_t wbs = kDefaultWriteBufferSize)
      : db_path(db),
        compress(c),
        block_size(bs),
        write_buffer_size(wbs) {}
  };

  explicit BinLogger(const Options& options);
  ~BinLogger();

  int64_t GetLength() const;
  bool ReadSlot(int64_t slot_index, LogEntry* log_entry);
  void AppendEntry(const LogEntry& log_entry);
  void Truncate(int64_t truncate_slot_index);
  void AppendEntryList(const ::google::protobuf::RepeatedPtrField<mpr::chubby::Entry>& entries);
  bool RemoveSlot(int64_t slot_index);
  bool RemoveSlotBefore(int64_t slot_gc_index);
  
  void GetLastLogIndexAndTerm(int64_t* last_log_index, int64_t* last_log_term) const;


  void LogEntryToString(const LogEntry& log_entry, std::string* result);
  void StringToLogEntry(const std::string& buf, LogEntry* result);

  // static
  static std::string IndexToKey(int64_t index);
  static int64_t KeyToIndex(const std::string& key);

 private:
  std::unique_ptr<leveldb::DB> db_;
  int64_t length_;
  int64_t last_log_term_;
  mutable base::mutex mu_;

  DISALLOW_COPY_AND_ASSIGN(BinLogger);
};

} // namespace chubby
} // namespace mpr
#endif // MPR_CHUBBY_STORAGE_BIN_LOGGER_H_
