#include "server/server.h"

#include <exception>

int main() {
    try {
        printf("MemVault is starting...\n");

        Server server;
        printf("Server socket successfully initialised and bound to port %d\n", 6379);

        server.run();
        printf("Listening on port 6379...\n");


    } catch (const std::exception &error) {
        fprintf(stderr, "Server error: %s", error.what());
        return 1;
    }

    return 0;
}
