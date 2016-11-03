#include "storage/bin_logger.h"
#include <gtest/gtest.h>

namespace mpr {
namespace chubby {

TEST(BinLogger, LogEntryConvert) {

  BinLogger bin_logger(BinLogger::Options("/tmp/"));
  LogEntry log_entry, log_entry2;
  log_entry.log_operation= kNop;
  log_entry.key = "abc";
  log_entry.value = "123";
  log_entry.term = 1;
  std::string buf;
  bin_logger.LogEntryToString(log_entry, &buf);
  EXPECT_EQ(buf.size(), 27u); //#1+4+0+4+3+4+3+8
  std::string buf2 = buf;
  bin_logger.StringToLogEntry(buf2, &log_entry2);
  EXPECT_EQ(log_entry.key, log_entry2.key);
  EXPECT_EQ(log_entry.value, log_entry2.value);
  EXPECT_EQ(log_entry.term, log_entry2.term);
  EXPECT_EQ(log_entry.log_operation, log_entry2.log_operation);
}

TEST(BinLogger, SlotWrite) {
  BinLogger bin_logger(BinLogger::Options("/tmp/"));
  char key_buf[1024] = {'\0'};
  char value_buf[1024] = {'\0'};
  for (int i=1; i<=100; i++) {
    LogEntry log_entry;
    snprintf(key_buf, sizeof(key_buf), "key_%d", i);
    snprintf(value_buf, sizeof(value_buf), "value_%d", i);
    log_entry.key = key_buf;
    log_entry.value = value_buf;
    log_entry.term = i;
    log_entry.log_operation= (i % 2 == 0) ? kPut : kDel;
    bin_logger.AppendEntry(log_entry);
  }    
}     

TEST(BinLogger, SlotBatchWrite) {
  BinLogger bin_logger(BinLogger::Options("/tmp/", true));
  char key_buf[1024] = {'\0'};
  char value_buf[1024] = {'\0'};
  ::google::protobuf::RepeatedPtrField< ::mpr::chubby::Entry > entries;
  for (int i=101; i<=200; i++) {
    Entry* log_entry = entries.Add();
    snprintf(key_buf, sizeof(key_buf), "key_%d", i);
    snprintf(value_buf, sizeof(value_buf), "value_%d", i);
    log_entry->set_key(key_buf);
    log_entry->set_value(value_buf);
    log_entry->set_term(i);
    log_entry->set_op((i % 2 == 0) ? kPut : kDel);
  }
  bin_logger.AppendEntryList(entries);
}
  
  
TEST(BinLogger, SlotRead) {
  BinLogger bin_logger(BinLogger::Options("/tmp/"));
  char key_buf[1024] = {'\0'};
  char value_buf[1024] = {'\0'};
  for (int i=1; i<=200; i++) {
    LogEntry log_entry;
    bin_logger.ReadSlot(i-1, &log_entry);
    snprintf(key_buf, sizeof(key_buf), "key_%d", i);
    snprintf(value_buf, sizeof(value_buf), "value_%d", i);
    EXPECT_EQ(log_entry.key, std::string(key_buf));
    EXPECT_EQ(log_entry.value, std::string(value_buf));
    EXPECT_EQ(log_entry.term, i);
    EXPECT_EQ(log_entry.log_operation, (i % 2 == 0) ? kPut : kDel);
  }
}


#if 0
TEST(BinLogger, SlotTruncate) {
  BinLogger bin_logger(BinLogger::Options("/tmp/"));
  EXPECT_EQ( bin_logger.GetLength(), 200 );
  bin_logger.Truncate(49);
  EXPECT_EQ( bin_logger.GetLength(), 50);
  bin_logger.Truncate(-1);
  EXPECT_EQ(bin_logger.GetLength(), 0);
}
#endif

} // namespace chubby
} // namespace mpr
