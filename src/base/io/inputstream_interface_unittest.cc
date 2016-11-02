#include "base/io/inputstream_interface.h"
#include "base/errors.h"
#include "base/status_test_util.h"
#include <gtest/gtest.h>

namespace base {
namespace io {
namespace {

class TestStringStream : public InputStreamInterface {
 public:
  TestStringStream(const string& content) : content_(content) {}

  Status ReadNBytes(int64 bytes_to_read, string* result) override {
    result->clear();
    if (pos_ + bytes_to_read > (int64)content_.size()) {
      return errors::OutOfRange("limit reached");
    }
    *result = content_.substr(pos_, bytes_to_read);
    pos_ += bytes_to_read;
    return Status::OK();
  }

  int64 Tell() const override { return pos_; }

  Status Reset() override {
    pos_ = 0;
    return Status::OK();
  }

 private:
  string content_;
  int64 pos_ = 0;
};

TEST(InputStreamInterface, Basic) {
  TestStringStream ss("This is a test string");
  string res;
  MPR_ASSERT_OK(ss.ReadNBytes(4, &res));
  EXPECT_EQ("This", res);
  MPR_ASSERT_OK(ss.SkipNBytes(6));
  MPR_ASSERT_OK(ss.ReadNBytes(11, &res));
  EXPECT_EQ("test string", res);
  // Skipping past end of the file causes OutOfRange error.
  EXPECT_TRUE(errors::IsOutOfRange(ss.SkipNBytes(1)));

  MPR_ASSERT_OK(ss.Reset());
  MPR_ASSERT_OK(ss.ReadNBytes(4, &res));
  EXPECT_EQ("This", res);
}

}  // anonymous namespace
}  // namespace io
}  // namespace base
