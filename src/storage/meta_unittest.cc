#include "base/io/inputbuffer.h"
#include "base/io/path.h"
#include "base/platform/env.h"

#include "storage/meta_file_interface.h"
#include "storage/meta_file.h"

#include <gtest/gtest.h>

// Format
//
// Term Append File:
// <int64>
// <int64>
// ...
// <int64>
//
// Vote Append File:
// <int64> <string>
// <int64> <string>
// ...
// <int64> <string>
//
// RootInfo ReadOnly
// <string> <string>
//

namespace mpr {
namespace chubby {

TEST(Term, ReadWrite) {
  std::string term_file_name = base::io::JoinPath("/tmp", "term_file");
  
  std::unique_ptr<MetaFileInterface> meta_file(new MetaFile(term_file_name));
  std::string read_line;
  std::string write_line;

  base::Status status;

  for (int i=0; i < 10; i++) {
    write_line = std::to_string(i) + "\n";
    status = meta_file->WriteLine(write_line);
    DCHECK(status.ok()) << status.ToString();

    status = meta_file->ReadLine(&read_line);
    DCHECK(status.ok() || status.code() == base::error::OUT_OF_RANGE) << status.ToString();

    EXPECT_EQ(write_line, read_line);
  }
  EXPECT_TRUE(true);
}

TEST(Vote, ReadWrite) {
  EXPECT_TRUE(true);
}

TEST(RootInfo, ReadWrite) {
  EXPECT_TRUE(true);
}

} // namespace chubby
} // namespace mpr
