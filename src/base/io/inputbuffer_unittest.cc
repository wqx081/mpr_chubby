#include "base/io/inputbuffer.h"

#include <vector>
#include "base/platform/env.h"
#include "base/coding.h"
#include "base/errors.h"
#include "base/status.h"
#include "base/status_test_util.h"
#include "base/strings/strcat.h"
#include "base/logging.h"
#include <gtest/gtest.h>

namespace base {
namespace {

static std::vector<int> BufferSizes() {
  return {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,   11,
          12, 13, 14, 15, 16, 17, 18, 19, 20, 65536};
}

TEST(InputBuffer, ReadLine_Empty) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "");

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string line;
    io::InputBuffer in(file.get(), buf_size);
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
  }
}

TEST(InputBuffer, ReadLine1) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "line one\nline two\nline three\n");

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string line;
    io::InputBuffer in(file.get(), buf_size);
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line one");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line two");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line three");
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
    // A second call should also return end of file
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
  }
}

TEST(InputBuffer, ReadLine_NoTrailingNewLine) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "line one\nline two\nline three");

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string line;
    io::InputBuffer in(file.get(), buf_size);
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line one");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line two");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line three");
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
    // A second call should also return end of file
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
  }
}

TEST(InputBuffer, ReadLine_EmptyLines) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "line one\n\n\nline two\nline three");

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string line;
    io::InputBuffer in(file.get(), buf_size);
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line one");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line two");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line three");
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
    // A second call should also return end of file
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
  }
}

TEST(InputBuffer, ReadLine_CRLF) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "line one\r\n\r\n\r\nline two\r\nline three");

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string line;
    io::InputBuffer in(file.get(), buf_size);
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line one");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line two");
    CHECK_OK(in.ReadLine(&line));
    EXPECT_EQ(line, "line three");
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
    // A second call should also return end of file
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadLine(&line)));
  }
}

TEST(InputBuffer, ReadNBytes) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "0123456789");

  // ReadNBytes(int64, string*).
  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string read;
    io::InputBuffer in(file.get(), buf_size);
    EXPECT_EQ(0, in.Tell());
    CHECK_OK(in.ReadNBytes(3, &read));
    EXPECT_EQ(read, "012");
    EXPECT_EQ(3, in.Tell());
    CHECK_OK(in.ReadNBytes(0, &read));
    EXPECT_EQ(read, "");
    EXPECT_EQ(3, in.Tell());
    CHECK_OK(in.ReadNBytes(4, &read));
    EXPECT_EQ(read, "3456");
    EXPECT_EQ(7, in.Tell());
    CHECK_OK(in.ReadNBytes(0, &read));
    EXPECT_EQ(read, "");
    EXPECT_EQ(7, in.Tell());
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(5, &read)));
    EXPECT_EQ(read, "789");
    EXPECT_EQ(10, in.Tell());
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(5, &read)));
    EXPECT_EQ(read, "");
    EXPECT_EQ(10, in.Tell());
    CHECK_OK(in.ReadNBytes(0, &read));
    EXPECT_EQ(read, "");
    EXPECT_EQ(10, in.Tell());
  }
  // ReadNBytes(int64, char*, size_t*).
  size_t bytes_read;
  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    char read[5];
    io::InputBuffer in(file.get(), buf_size);

    EXPECT_EQ(0, in.Tell());
    MPR_ASSERT_OK(in.ReadNBytes(3, read, &bytes_read));
    EXPECT_EQ(StringPiece(read, 3), "012");

    EXPECT_EQ(3, in.Tell());
    MPR_ASSERT_OK(in.ReadNBytes(0, read, &bytes_read));
    EXPECT_EQ(StringPiece(read, 3), "012");

    EXPECT_EQ(3, in.Tell());
    MPR_ASSERT_OK(in.ReadNBytes(4, read, &bytes_read));
    EXPECT_EQ(StringPiece(read, 4), "3456");

    EXPECT_EQ(7, in.Tell());
    MPR_ASSERT_OK(in.ReadNBytes(0, read, &bytes_read));
    EXPECT_EQ(StringPiece(read, 4), "3456");

    EXPECT_EQ(7, in.Tell());
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(5, read, &bytes_read)));
    EXPECT_EQ(StringPiece(read, 3), "789");

    EXPECT_EQ(10, in.Tell());
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(5, read, &bytes_read)));
    EXPECT_EQ(StringPiece(read, 3), "789");

    EXPECT_EQ(10, in.Tell());
    MPR_ASSERT_OK(in.ReadNBytes(0, read, &bytes_read));
    EXPECT_EQ(StringPiece(read, 3), "789");
    EXPECT_EQ(10, in.Tell());
  }
}

TEST(InputBuffer, SkipNBytes) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "0123456789");

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string read;
    io::InputBuffer in(file.get(), buf_size);
    EXPECT_EQ(0, in.Tell());
    CHECK_OK(in.SkipNBytes(3));
    EXPECT_EQ(3, in.Tell());
    CHECK_OK(in.SkipNBytes(0));
    EXPECT_EQ(3, in.Tell());
    CHECK_OK(in.ReadNBytes(2, &read));
    EXPECT_EQ(read, "34");
    EXPECT_EQ(5, in.Tell());
    CHECK_OK(in.SkipNBytes(0));
    EXPECT_EQ(5, in.Tell());
    CHECK_OK(in.SkipNBytes(2));
    EXPECT_EQ(7, in.Tell());
    CHECK_OK(in.ReadNBytes(1, &read));
    EXPECT_EQ(read, "7");
    EXPECT_EQ(8, in.Tell());
    EXPECT_TRUE(errors::IsOutOfRange(in.SkipNBytes(5)));
    EXPECT_EQ(10, in.Tell());
    EXPECT_TRUE(errors::IsOutOfRange(in.SkipNBytes(5)));
    EXPECT_EQ(10, in.Tell());
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(5, &read)));
    EXPECT_EQ(read, "");
    EXPECT_EQ(10, in.Tell());
  }
}

TEST(InputBuffer, Seek) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";
  WriteStringToFile(env, fname, "0123456789");

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    string read;
    io::InputBuffer in(file.get(), buf_size);

    CHECK_OK(in.ReadNBytes(3, &read));
    EXPECT_EQ(read, "012");
    CHECK_OK(in.ReadNBytes(3, &read));
    EXPECT_EQ(read, "345");

    CHECK_OK(in.Seek(0));
    CHECK_OK(in.ReadNBytes(3, &read));
    EXPECT_EQ(read, "012");

    CHECK_OK(in.Seek(3));
    CHECK_OK(in.ReadNBytes(4, &read));
    EXPECT_EQ(read, "3456");

    CHECK_OK(in.Seek(4));
    CHECK_OK(in.ReadNBytes(4, &read));
    EXPECT_EQ(read, "4567");

    CHECK_OK(in.Seek(1 << 25));
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadNBytes(1, &read)));

    EXPECT_TRUE(
        StringPiece(in.Seek(-1).ToString()).contains("negative position"));
  }
}

TEST(InputBuffer, ReadVarint32) {
  Env* env = Env::Default();
  string fname = std::string("/tmp") + "/inputbuffer_test";

  // Generates data.
  std::vector<uint32> data;
  uint32 i = 0;
  for (; i < (1 << 10); i += 1) data.push_back(i);
  for (; i < (1 << 15); i += 5) data.push_back(i);
  for (; i < (uint32)(1 << 31); i += 132817) data.push_back(i);
  data.push_back(std::numeric_limits<uint32>::max());

  // Writes the varints.
  {
    std::unique_ptr<WritableFile> file;
    CHECK_OK(env->NewWritableFile(fname, &file));
    string varint;
    for (uint32 number : data) {
      varint.clear();
      PutVarint32(&varint, number);
      CHECK_OK(file->Append(StringPiece(varint)));
    }
  }

  for (auto buf_size : BufferSizes()) {
    std::unique_ptr<RandomAccessFile> file;
    CHECK_OK(env->NewRandomAccessFile(fname, &file));
    io::InputBuffer in(file.get(), buf_size);
    uint32 result = 0;

    for (uint32 expected : data) {
      MPR_ASSERT_OK(in.ReadVarint32(&result));
      EXPECT_EQ(expected, result);
    }
    EXPECT_TRUE(errors::IsOutOfRange(in.ReadVarint32(&result)));
  }
}

}  // namespace
}  // namespace base
