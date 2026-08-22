#include "server/server.h"

#include <exception>
#include <iostream>

int main() {
  try {
    std::cout << "MemVault is starting...\n";

    Server server;
    std::cout << "Server socket successfully initialised and bound to port 6379!\n";

    server.run();
    std::cout << "Listening on port 6379...\n";

  } catch (const std::exception& error) {
    std::cerr << "Server error: " << error.what() << "\n";
    return 1;
  }

  return 0;
}
