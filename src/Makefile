CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -I/usr/local/include/evhtp/
CXXFLAGS += -std=c++11 -Wall -Werror -g -c -o

LIB_FILES :=-lglog -lgflags -levent_openssl -levent -levent_pthreads -lz \
	-lpthread \
	-lleveldb \
	-lprotobuf \
	-ldl \
	-lz \
	-lsnappy \
	-levhtp	-lssl -lcrypto -levent_openssl \

TEST_LIB_FILES :=  -L/usr/local/lib -lgtest -lgtest_main -lgmock -lpthread

PROTOC = protoc
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ./protos

CPP_SOURCES := \
	\
	./base/string_encode.cc \
	./base/stringpiece.cc \
	./base/status.cc \
	./base/arena.cc \
	./base/bitmap.cc \
	./base/coding.cc \
	./base/histogram.cc \
	./base/threadpool.cc \
	\
	./base/monitoring/registry.cc \
	./base/monitoring/util/protobuf.cc \
	./base/monitoring/prometheus/metrics.pb.cc \
	./base/monitoring/prometheus/exporter.cc \
	\
	./base/strings/stringprintf.cc \
	./base/strings/numbers.cc \
	./base/strings/scanner.cc \
	./base/strings/strcat.cc \
	./base/strings/str_util.cc \
	./base/strings/base64.cc \
	\
	./base/ini/ini.cc \
	./base/ini/ini_reader.cc \
	\
	./base/hash/hash.cc \
	./base/hash/crc32c.cc \
	\
	./base/random/distribution_sampler.cc \
	./base/random/random.cc \
	./base/random/simple_philox.cc \
	./base/random/weighted_picker.cc \
	\
	./base/platform/mem.cc \
	./base/platform/env.cc \
	./base/platform/file_system.cc \
	./base/platform/linux/linux_file_system.cc \
	./base/platform/linux/env.cc \
	./base/platform/linux/load_library.cc \
	./base/platform/linux/error.cc \
	./base/platform/snappy.cc \
	\
	./base/io/path.cc \
	./base/io/inputstream_interface.cc \
	./base/io/inputbuffer.cc \
	./base/io/random_inputstream.cc \
	./base/io/buffered_inputstream.cc \
	./base/io/zlib_inputstream.cc \
	./base/io/zlib_outputbuffer.cc \
	\
	./base/io/table/block.cc \
	./base/io/table/block_builder.cc \
	./base/io/table/format.cc \
	./base/io/table/iterator.cc \
	./base/io/table/table.cc \
	./base/io/table/table_builder.cc \
	./base/io/table/two_level_iterator.cc \
	\
	./base/http/http_parser.cc \
	\
	\
	./proto/service.pb.cc \
	./storage/bin_logger.cc \
	./storage/database.cc \
	./server/flags.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./base/http/libevhtp_basic_unittest \
	./base/http/libevhtp_http_server_unittest \
	./base/libevent/http_client_unittest \
	\
	\
	./storage/database_unittest \
	./storage/bin_logger_unittest \

#APP := mpr_rest_server
APP := #mpr_rest_server

all: $(CPP_OBJECTS) $(APP) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

$(APP): ./server/http_server.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./server/http_server.o: \
	./server/http_server.cc \
	./server/handler_context.h \
	./server/http_server.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./server/handler_chain_unittest: \
	./server/handler_chain_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./server/handler_chain_unittest.o: \
	./server/handler_chain_unittest.cc \
	./server/handler_chain.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

## base
./base/threadpool_unittest: ./base/threadpool_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/threadpool_unittest.o: ./base/threadpool_unittest.cc \
	./base/threadpool.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/notification_unittest: ./base/notification_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/notification_unittest.o: ./base/notification_unittest.cc \
	./base/notification.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/arena_unittest: ./base/arena_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/arena_unittest.o: ./base/arena_unittest.cc \
	./base/arena.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/bitmap_unittest: ./base/bitmap_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/bitmap_unittest.o: ./base/bitmap_unittest.cc \
	./base/bitmap.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/histogram_unittest: ./base/histogram_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/histogram_unittest.o: ./base/histogram_unittest.cc \
	./base/histogram.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/ref_counted_unittest: ./base/ref_counted_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/ref_counted_unittest.o: \
	./base/ref_counted_unittest.cc \
	./base/ref_counted.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/math_util_unittest: ./base/math_util_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/math_util_unittest.o: ./base/math_util_unittest.cc \
	./base/math_util.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/gtl/array_slice_unittest: ./base/gtl/array_slice_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/array_slice_unittest.o: ./base/gtl/array_slice_unittest.cc \
	./base/gtl/array_slice_internal.h \
	./base/gtl/array_slice.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/cleanup_unittest: ./base/gtl/cleanup_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/cleanup_unittest.o: ./base/gtl/cleanup_unittest.cc \
	./base/gtl/cleanup.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/edit_distance_unittest: ./base/gtl/edit_distance_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/edit_distance_unittest.o: ./base/gtl/edit_distance_unittest.cc \
	./base/gtl/edit_distance.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/inlined_vector_unittest: ./base/gtl/inlined_vector_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/inlined_vector_unittest.o: ./base/gtl/inlined_vector_unittest.cc \
	./base/gtl/inlined_vector.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/int_type_unittest: ./base/gtl/int_type_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/int_type_unittest.o: ./base/gtl/int_type_unittest.cc \
	./base/gtl/int_type.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/iterator_range_unittest: ./base/gtl/iterator_range_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/iterator_range_unittest.o: ./base/gtl/iterator_range_unittest.cc \
	./base/gtl/iterator_range.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/manual_constructor_unittest: ./base/gtl/manual_constructor_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/manual_constructor_unittest.o: ./base/gtl/manual_constructor_unittest.cc \
	./base/gtl/manual_constructor.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/map_util_unittest: ./base/gtl/map_util_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/map_util_unittest.o: ./base/gtl/map_util_unittest.cc \
	./base/gtl/map_util.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/top_n_unittest: ./base/gtl/top_n_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/top_n_unittest.o: ./base/gtl/top_n_unittest.cc \
	./base/gtl/top_n.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/strings/numbers_unittest: ./base/strings/numbers_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/numbers_unittest.o: ./base/strings/numbers_unittest.cc \
	./base/strings/numbers.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/scanner_unittest: ./base/strings/scanner_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/scanner_unittest.o: ./base/strings/scanner_unittest.cc \
	./base/strings/scanner.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/stringprintf_unittest: ./base/strings/stringprintf_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/stringprintf_unittest.o: ./base/strings/stringprintf_unittest.cc \
	./base/strings/stringprintf.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/strcat_unittest: ./base/strings/strcat_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/strcat_unittest.o: ./base/strings/strcat_unittest.cc \
	./base/strings/strcat.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/str_util_unittest: ./base/strings/str_util_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/str_util_unittest.o: ./base/strings/str_util_unittest.cc \
	./base/strings/str_util.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/hash/crc32c_unittest: ./base/hash/crc32c_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/hash/crc32c_unittest.o: ./base/hash/crc32c_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/hash/hash_unittest: ./base/hash/hash_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/hash/hash_unittest.o: ./base/hash/hash_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/random/distribution_sampler_unittest: ./base/random/distribution_sampler_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/distribution_sampler_unittest.o: ./base/random/distribution_sampler_unittest.cc \
	./base/random/distribution_sampler.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/philox_random_unittest: ./base/random/philox_random_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/philox_random_unittest.o: ./base/random/philox_random_unittest.cc \
	./base/random/philox_random.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/random_distributions_unittest: ./base/random/random_distributions_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/random_distributions_unittest.o: ./base/random/random_distributions_unittest.cc \
	./base/random/random_distributions.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/random_unittest: ./base/random/random_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/random_unittest.o: ./base/random/random_unittest.cc \
	./base/random/random.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/simple_philox_unittest: ./base/random/simple_philox_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/simple_philox_unittest.o: ./base/random/simple_philox_unittest.cc \
	./base/random/simple_philox.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/weighted_picker_unittest: ./base/random/weighted_picker_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/weighted_picker_unittest.o: ./base/random/weighted_picker_unittest.cc \
	./base/random/weighted_picker.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<


./base/monitoring/counter_unittest: ./base/monitoring/counter_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/monitoring/counter_unittest.o: ./base/monitoring/counter_unittest.cc \
	./base/monitoring/counter.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/monitoring/gauge_unittest: ./base/monitoring/gauge_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/monitoring/gauge_unittest.o: ./base/monitoring/gauge_unittest.cc \
	./base/monitoring/gauge.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/monitoring/registry_unittest: ./base/monitoring/registry_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/monitoring/registry_unittest.o: ./base/monitoring/registry_unittest.cc \
	./base/monitoring/registry.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/io/buffered_inputstream_unittest: ./base/io/buffered_inputstream_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/io/buffered_inputstream_unittest.o: ./base/io/buffered_inputstream_unittest.cc \
	./base/io/buffered_inputstream.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/io/path_unittest: ./base/io/path_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/io/path_unittest.o: ./base/io/path_unittest.cc \
	./base/io/path.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/io/inputbuffer_unittest: ./base/io/inputbuffer_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/io/inputbuffer_unittest.o: ./base/io/inputbuffer_unittest.cc \
	./base/io/inputbuffer.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/io/inputstream_interface_unittest: ./base/io/inputstream_interface_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/io/inputstream_interface_unittest.o: ./base/io/inputstream_interface_unittest.cc \
	./base/io/inputstream_interface.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/io/zlib_buffers_unittest: ./base/io/zlib_buffers_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/io/zlib_buffers_unittest.o: ./base/io/zlib_buffers_unittest.cc \
	./base/io/zlib_inputstream.h \
	./base/io/zlib_outputbuffer.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/io/random_inputstream_unittest: ./base/io/random_inputstream_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/io/random_inputstream_unittest.o: ./base/io/random_inputstream_unittest.cc \
	./base/io/random_inputstream.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/io/table/table_unittest: ./base/io/table/table_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/io/table/table_unittest.o: ./base/io/table/table_unittest.cc \
	./base/io/table/block.h \
	./base/io/table/block_builder.h \
	./base/io/table/table.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/http/http_parser_unittest: ./base/http/http_parser_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/http/http_parser_unittest.o: ./base/http/http_parser_unittest.cc \
	./base/http/http_parser.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/http/libevhtp_basic_unittest: ./base/http/libevhtp_basic_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/http/libevhtp_basic_unittest.o: ./base/http/libevhtp_basic_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/http/libevhtp_http_server_unittest: ./base/http/libevhtp_http_server_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/http/libevhtp_http_server_unittest.o: ./base/http/libevhtp_http_server_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/libevent/http_client_unittest: ./base/libevent/http_client_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/libevent/http_client_unittest.o: ./base/libevent/http_client_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<



./storage/database_unittest: ./storage/database_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./storage/database_unittest.o: ./storage/database_unittest.cc \
	./storage/database.o
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./storage/bin_logger_unittest: ./storage/bin_logger_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./storage/bin_logger_unittest.o: ./storage/bin_logger_unittest.cc \
	./storage/bin_logger.o
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

## /////////////////////////////

clean:
	find . -name "*.o" | xargs rm
	rm -fr $(APP)
	@rm -fr $(TESTS)
	@echo "rm *_unittest"
	@rm -fr $(CPP_OBJECTS)
	@echo "rm *.o"
