#include "server.h"
#include <sys/socket.h>

Server::Server (int port) : port(port) {
  // Creating a server socket
  // AF_INET for IPv4 (AF_INET6 for IPv6 or dual-stack sockets)
  // SOCK_STREAM for TCP
  // protocol set to 0 for auto
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  // Set socket options
  // bind() function
  int val = 1;
  // Set SO_REUSEADDR to value 1
  // if not, server program cannot bind to the same IP:Port it was using after restart
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}
