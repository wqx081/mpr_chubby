#include "base/stringpiece.h"
#include "base/strings/strcat.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <glog/logging.h>
#include <gtest/gtest.h>

namespace base {
namespace libevent {

TEST(HTTPClient, Sync) {

  const std::string query = strings::StrCat("GET / HTTP/1.1\r\n",
                                            "Host: www.baidu.com\r\n",
                                            "\r\n");
  const StringPiece hostname("www.baidu.com");
  struct sockaddr_in sin;
  struct hostent* h;
  int fd;
  ssize_t n_written, remaining;
  size_t cursor = 0;
  char buf[1024];

  h = gethostbyname(hostname.data());
  DCHECK(h) << "Couldn't lookup: " << hostname << " : " << hstrerror(h_errno);
  
  DCHECK(h->h_addrtype == AF_INET) << "No ipv6 support, sorry";
  
  fd = socket(AF_INET, SOCK_STREAM, 0);
  DCHECK(fd > 0) << "socket" << strerror(errno);
  
  sin.sin_family = AF_INET;
  sin.sin_port = htons(80);
  memcpy(&sin.sin_addr, h->h_addr, sizeof(sin.sin_addr));
  if (connect(fd, (struct sockaddr*)&sin, sizeof(sin))) {
    perror("connect");
    close(fd);
    EXPECT_TRUE(false);
  }

  remaining = query.size();
  cursor = 0;
  while (remaining) {
    n_written = send(fd, query.data() + cursor, remaining, 0);
//    DCHECK(n_written > 0) << strerror(errno);
    remaining -= n_written;
    cursor += n_written;
  }

  while (1) {
    ssize_t result = recv(fd, buf, sizeof(buf), 0);
    if (result <= 0) break;
//    DCHECK(result > 0) << strerror(errno);
    LOG(INFO) << StringPiece(buf, result);
  }

  close(fd);

  EXPECT_TRUE(true);
}

} // namespace event
} // namespace base
