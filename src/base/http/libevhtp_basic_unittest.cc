#include "base/port.h"
#include <evhtp.h>

#include <gtest/gtest.h>

namespace base {
namespace http {

void TestCallback(evhtp_request_t* request, void* a) {
  const char* str = static_cast<char *>(a);

  evbuffer_add_printf(request->buffer_out, "%s", str);
  evhtp_send_reply(request, EVHTP_RES_OK);
}

TEST(Libevhtp, Basic) {

 evbase_t* evbase = event_base_new();
 evhtp_t* htp    = evhtp_new(evbase, NULL);
  
 evhtp_set_cb(htp, "/simple/", TestCallback, (void *)"simple");
 evhtp_set_cb(htp, "/1/ping", TestCallback, (void *)"one");
 evhtp_set_cb(htp, "/1/ping.json", TestCallback, (void *)"two");

 evhtp_bind_socket(htp, "0.0.0.0", 8081, 2048);
  
 event_base_loop(evbase, 0);
  
 evhtp_unbind_socket(htp);
 evhtp_free(htp);
 event_base_free(evbase);
}

} // namespace http
} // namespace base
