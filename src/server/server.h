#pragma once

class Server {
  private:
    int port;
    int server_fd{-1};

  public:
    explicit Server(int port);
    ~Server();

    void run();
};
