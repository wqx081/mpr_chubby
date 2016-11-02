#include "base/io/random_inputstream.h"
#include "base/status_test_util.h"
#include "base/platform/env.h"
#include <gtest/gtest.h>

namespace base {
namespace io {
namespace {

TEST(RandomInputStream, ReadNBytes) {
  Env* env = Env::Default();
  string fname =  "/tmp/random_inputbuffer_test";
  WriteStringToFile(env, fname, "0123456789");

  std::unique_ptr<RandomAccessFile> file;
  MPR_ASSERT_OK(env->NewRandomAccessFile(fname, &file));
  string read;
  RandomAccessInputStream in(file.get());
  MPR_ASSERT_OK(in.ReadNBytes(3, &read));
  EXPECT_EQ(read, "012");
  EXPECT_EQ(3, in.Tell());
  MPR_ASSERT_OK(in.ReadNBytes(0, &read));
  EXPECT_EQ(read, "");
  EXPECT_EQ(3, in.Tell());
  MPR_ASSERT_OK(in.ReadNBytes(5, &read));
  EXPECT_EQ(read, "34567");
  EXPECT_EQ(8, in.Tell());
  MPR_ASSERT_OK(in.ReadNBytes(0, &read));
  EXPECT_EQ(read, "");
  EXPECT_EQ(8, in.Tell());
  EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(20, &read)));
  EXPECT_EQ(read, "89");
  EXPECT_EQ(10, in.Tell());
  MPR_ASSERT_OK(in.ReadNBytes(0, &read));
  EXPECT_EQ(read, "");
  EXPECT_EQ(10, in.Tell());
}

TEST(RandomInputStream, SkipNBytes) {
  Env* env = Env::Default();
  string fname = "/tmp/random_inputbuffer_test";
  WriteStringToFile(env, fname, "0123456789");

  std::unique_ptr<RandomAccessFile> file;
  MPR_ASSERT_OK(env->NewRandomAccessFile(fname, &file));
  string read;
  RandomAccessInputStream in(file.get());
  MPR_ASSERT_OK(in.SkipNBytes(3));
  EXPECT_EQ(3, in.Tell());
  MPR_ASSERT_OK(in.ReadNBytes(0, &read));
  EXPECT_EQ(read, "");
  EXPECT_EQ(3, in.Tell());
  MPR_ASSERT_OK(in.ReadNBytes(4, &read));
  EXPECT_EQ(read, "3456");
  EXPECT_EQ(7, in.Tell());
  MPR_ASSERT_OK(in.SkipNBytes(0));
  EXPECT_EQ(7, in.Tell());
  MPR_ASSERT_OK(in.ReadNBytes(2, &read));
  EXPECT_EQ(read, "78");
  EXPECT_EQ(9, in.Tell());
  EXPECT_TRUE(errors::IsOutOfRange(in.SkipNBytes(20)));
  EXPECT_EQ(10, in.Tell());
  // Making sure that if we read after we've skipped beyond end of file, we get
  // nothing.
  EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(5, &read)));
  EXPECT_EQ(read, "");
  EXPECT_EQ(10, in.Tell());
}

TEST(RandomInputStream, Seek) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/random_inputbuffer_seek_test";
  WriteStringToFile(env, fname, "0123456789");

  std::unique_ptr<RandomAccessFile> file;
  MPR_ASSERT_OK(env->NewRandomAccessFile(fname, &file));
  string read;
  RandomAccessInputStream in(file.get());

  // Seek forward
  MPR_ASSERT_OK(in.Seek(3));
  EXPECT_EQ(3, in.Tell());

  // Read 4 bytes
  MPR_ASSERT_OK(in.ReadNBytes(4, &read));
  EXPECT_EQ(read, "3456");
  EXPECT_EQ(7, in.Tell());

  // Seek backwards
  MPR_ASSERT_OK(in.Seek(1));
  MPR_ASSERT_OK(in.ReadNBytes(4, &read));
  EXPECT_EQ(read, "1234");
  EXPECT_EQ(5, in.Tell());
}

}  // anonymous namespace
}  // namespace io
}  // namespace base
