#ifndef BASE_IO_TABLE_TWO_LEVEL_ITERATOR_H_
#define BASE_IO_TABLE_TWO_LEVEL_ITERATOR_H_

#include "base/io/table/iterator.h"

namespace base {
namespace table {

// Return a new two level iterator.  A two-level iterator contains an
// index iterator whose values point to a sequence of blocks where
// each block is itself a sequence of key,value pairs.  The returned
// two-level iterator yields the concatenation of all key/value pairs
// in the sequence of blocks.  Takes ownership of "index_iter" and
// will delete it when no longer needed.
//
// Uses a supplied function to convert an index_iter value into
// an iterator over the contents of the corresponding block.
extern Iterator* NewTwoLevelIterator(
    Iterator* index_iter,
    Iterator* (*block_function)(void* arg, const StringPiece& index_value),
    void* arg);

}  // namespace table
}  // namespace base

#endif // BASE_IO_TABLE_TWO_LEVEL_ITERATOR_H_
