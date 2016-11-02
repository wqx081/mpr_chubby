#ifndef BASE_IO_TABLE_BLOCK_BUILDER_H_
#define BASE_IO_TABLE_BLOCK_BUILDER_H_

#include <vector>

#include <stdint.h>
#include "base/stringpiece.h"

namespace base {
namespace table {

struct Options;

class BlockBuilder {
 public:
  explicit BlockBuilder(const Options* options);

  // Reset the contents as if the BlockBuilder was just constructed.
  void Reset();

  // REQUIRES: Finish() has not been called since the last call to Reset().
  // REQUIRES: key is larger than any previously added key
  void Add(const StringPiece& key, const StringPiece& value);

  // Finish building the block and return a slice that refers to the
  // block contents.  The returned slice will remain valid for the
  // lifetime of this builder or until Reset() is called.
  StringPiece Finish();

  // Returns an estimate of the current (uncompressed) size of the block
  // we are building.
  size_t CurrentSizeEstimate() const;

  // Return true iff no entries have been added since the last Reset()
  bool empty() const { return buffer_.empty(); }

 private:
  const Options* options_;
  string buffer_;                 // Destination buffer
  std::vector<uint32> restarts_;  // Restart points
  int counter_;                   // Number of entries emitted since restart
  bool finished_;                 // Has Finish() been called?
  string last_key_;

  // No copying allowed
  BlockBuilder(const BlockBuilder&);
  void operator=(const BlockBuilder&);
};

}  // namespace table
}  // namespace base

#endif
