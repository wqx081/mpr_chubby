#include "base/macros.h"
#include "base/port.h"
#include "base/logging.h"

#include <event2/event.h>
#include <evhtp.h>

#include <gtest/gtest.h>

#include <memory>

namespace base {
namespace http {

struct Options {
  int num_threads = 0;
  std::string bind_address;
  int bind_port;
  std::string ssl_pem;
  std::string ssl_ca;
  std::string bandwidth_limit;
  int max_keepalives = 1024;

  static Options Default() {
    static Options options;
    options.num_threads = 0;
    options.bind_address = "0.0.0.0";
    options.bind_port = 9980;
    options.ssl_pem = "";
    options.ssl_ca = "";
    return options;
  }
};

///////// Callback
//
evhtp_res OnHeaderComplete(evhtp_request_t* /* request */, 
                           evhtp_header_t* header, 
                           void* /* arg */ ) {
  LOG(INFO) << "----- OnHeaderComplete";
  LOG(INFO) << "key = " << header->key << " , value = " << header->val;
  return EVHTP_RES_OK;                                                                                  
}

evhtp_res OnHeadersComplete(evhtp_request_t* /* request */,
                            evhtp_headers_t* headers,
                            void* /* arg */) {
  LOG(INFO) << "----- OnHeadersComplete";
  evhtp_kvs_for_each(headers, [](evhtp_kv_t* header, void* /* arg */) -> int {
    LOG(INFO) << "key = " << header->key << " , value = " << header->val;
    return 0;
  }, nullptr);
  return EVHTP_RES_OK;
}


evhtp_res OnPathComplete(evhtp_request_t* request,
                         evhtp_path_t* path,
                         void* /* arg */) {
  LOG(INFO) << "----- OnPathComplete";
  LOG(INFO) << "full = " << path->full;
  LOG(INFO) << "path = " << path->path;
  LOG(INFO) << "file = " << path->file;
  LOG(INFO) << "match start = " << path->match_start;
  LOG(INFO) << "match end   = " << path->match_end;
  LOG(INFO) << "method      = " << evhtp_request_get_method(request);

  return EVHTP_RES_OK;
}

#if 0
OnReadBodyCallback // print_data
OnNewChunk // print_new_chunk_len
OnChunkComplete // print_chunk_complete
OnChunksComplete // print_chunks_completes
#endif

void GenCallback(evhtp_request_t* request, 
                 void* /* arg */) {

  LOG(INFO) << "----- GlobCallback";
  evbuffer_add(request->buffer_out, "Hello, World\n", 13);
  evhtp_send_reply(request, EVHTP_RES_OK);
}

evhtp_res PreAcceptCallback(evhtp_connection_t* /* conn */, 
                            void* /* arg */) {
  LOG(INFO) << "----- PreAcceptCallback";
  // return EVHTP_RES_ERROR;
  return EVHTP_RES_OK;
}

evhtp_res PostAcceptCallback(evhtp_connection_t* conn , void* /* arg */) {

  // 已经 accept 一个连接，要开始在此连接上注册解析回调函数
  LOG(INFO) << "----- PostAcceptCallback";

  evhtp_set_hook(&conn->hooks, evhtp_hook_type::evhtp_hook_on_header, 
                 (evhtp_hook)OnHeaderComplete, nullptr);
  evhtp_set_hook(&conn->hooks, evhtp_hook_type::evhtp_hook_on_headers,
                 (evhtp_hook)OnHeadersComplete, nullptr);
  evhtp_set_hook(&conn->hooks, evhtp_hook_type::evhtp_hook_on_path,
                 (evhtp_hook)OnPathComplete, nullptr);

  return EVHTP_RES_OK;
}

/////////////////////////////////////

class HTTPServer {
 public:
  explicit HTTPServer(const Options& options);
    

  void Start();
  void Stop();

 private:
  void Init() {
    ev_base_ = event_base_new();
    htp_ = evhtp_new(ev_base_, nullptr);
    DCHECK(ev_base_);
    DCHECK(htp_);
    evhtp_set_gencb(htp_, GenCallback, nullptr);
    evhtp_set_pre_accept_cb(htp_, PreAcceptCallback, nullptr);
    evhtp_set_post_accept_cb(htp_, PostAcceptCallback, nullptr);
  }

  const Options& options_;

  evbase_t* ev_base_;
  evhtp_t*  htp_;
  
  DISALLOW_COPY_AND_ASSIGN(HTTPServer);
};


HTTPServer::HTTPServer(const Options& options)
    : options_(options) {
  Init();
}

void HTTPServer::Start() {
  LOG(INFO) << "Server Run at: " << options_.bind_address << ": " << options_.bind_port;
  evhtp_bind_socket(htp_, 
                    options_.bind_address.c_str(), 
                    options_.bind_port, 2046);
  // TODO(wqx): Add signal handler
  event_base_loop(ev_base_, 0);
}

void HTTPServer::Stop() {
}


//////////////////////////////////
//
TEST(HTTPServer, Basic) {
  HTTPServer server(Options::Default());
  server.Start();
}

} // namespace http
} // namespace base
