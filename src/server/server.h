#pragma once

#include <cstdint>
#include <stdio.h>

class Server {
  private:
    int port;
    int server_fd{-1};

  public:
    explicit Server();

    static int32_t read_full(int fd, char* buf, size_t n);
    static int32_t write_full(int fd, char* buf, size_t n);

    void run();

    ~Server();
};
