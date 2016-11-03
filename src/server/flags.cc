#include <gflags/gflags.h>

DEFINE_bool(chubby_data_compress, true, "enable snappy compression on leveldb storage");
DEFINE_int32(chubby_data_block_size, 4, "for data, leveldb block_size, KB");
DEFINE_int32(chubby_data_write_buffer_size, 4, "for data, leveldb write_buffer_size, MB");

