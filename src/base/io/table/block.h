#ifndef BASE_IO_TABLE_BLOCK_H_
#define BASE_IO_TABLE_BLOCK_H_

#include <stddef.h>
#include <stdint.h>

#include "base/io/table/iterator.h"

namespace base {
namespace table {

struct BlockContents;

class Block {
 public:
  // Initialize the block with the specified contents.
  explicit Block(const BlockContents& contents);

  ~Block();

  size_t size() const { return size_; }
  Iterator* NewIterator();

 private:
  uint32 NumRestarts() const;

  const char* data_;
  size_t size_;
  uint32 restart_offset_;  // Offset in data_ of restart array
  bool owned_;             // Block owns data_[]

  // No copying allowed
  Block(const Block&);
  void operator=(const Block&);

  class Iter;
};

}  // namespace table
}  // namespace base

#endif  // BASE_IO_TABLE_BLOCK_H_
