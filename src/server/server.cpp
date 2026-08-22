#include "server.h"

#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

Server::Server() {
  port = 6379;

  // Creating a server socket
  // AF_INET for IPv4 (AF_INET6 for IPv6 or dual-stack sockets)
  // SOCK_STREAM for TCP
  // protocol set to 0 for auto
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1) {
    throw std::runtime_error("Failed to create server socket");
  }

  // Set socket options
  // bind() function
  int val = 1;
  // Set SO_REUSEADDR to value 1
  // if not, server program cannot bind to the same IP:Port it was using after
  // restart
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) ==
      -1) {
    close(server_fd);
    server_fd = -1;
    throw std::runtime_error("Failed to configure server socket");
  }

  /*
   * struct sockaddr_in {
   *  uint16_t sin_family; // AF_INET
   *  uint16_t sin_port; // port in big-endian
   *  struct in_addr sin_addr; // IPv4
   * }
   *
   * struct in_addr {
   *  uint32_t s_addr; // IPv4 in big-endian
   * }
   */
  sockaddr_in address{};

  address.sin_family = AF_INET;
  address.sin_port =
      htons(port); // Converts port into big-endian for network byte order
  address.sin_addr.s_addr =
      htonl(INADDR_LOOPBACK); // htonl converts the address to big-endian (Host
                              // to Network Long) host is the CPU endian
                              // (small-endian) Network is the big-endian.
                              // INADDR_LOOPBACK defines the address 127.0.0.1

  if (bind(server_fd,
           (const sockaddr *)&address, // converts IPv4 address pointer into
                                       // older generic socket API pointer type
           sizeof(address)) == -1) {
    close(server_fd);
    server_fd = -1;
    throw std::runtime_error("Failed to bind server socket");
  }

  if (listen(server_fd, SOMAXCONN) == -1) {
    close(server_fd);
    server_fd = -1;
    throw std::runtime_error("Failed to listen on server socket!");
  }
}

Server::~Server() {
  if (server_fd != -1) {
    close(server_fd);
  }
}

void Server::run() {
  std::cout << "Listening on port " << port << "...\n";

  while (true) {
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);

    if (client_fd < 0) {
      std::cerr << "Failed to accept client\n";
      continue; // we do not want to continue using this client socket
    }

    std::cout << "Client Connected!\n";

    char buffer[1024]{};

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
      std::string request(buffer, bytes_received);

      // TODO: Hard coded RESP commands, in future use it to decode
      const std::string resp_ping = "*1\r\n$4\r\nPING\r\n";
      const std::string inline_ping = "PING\r\n";

      std::string response;

      if (request == resp_ping || request == inline_ping) {
        response = "+PONG\r\n";
      } else {
        response = "-ERR unknown command\r\n";
      }

      if (send(client_fd, response.data(), response.size(), 0) == -1) {
        std::cerr << "Failed to send response!\n";
      }

    } else if (bytes_received == 0) {
      std::cout << "Client disconnected without sending data\n";
    } else {
      std::cerr << "Failed to receive data\n";
    }

    // TODO: Connection closes immediately after one response
    // => Support multiple requests on one conn
    close(client_fd);
  }
}
