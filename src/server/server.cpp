#include "server.h"

#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port) : port(port) {
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

  sockaddr_in address{};

  address.sin_family = AF_INET;
  address.sin_port = htons(port); // Converts port into network byte order
  address.sin_addr.s_addr =
      htonl(INADDR_ANY); // Accept any connection on any local IPv4 address

  if (bind(server_fd,
           reinterpret_cast<const sockaddr *>(
               &address), // converts IPv4 address pointer into older generic
                          // socket API pointer type
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
    int client_fd = accept(server_fd, nullptr, nullptr);

    if (client_fd == -1) {
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
