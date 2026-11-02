#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

static void error_msg(const char* msg) {
    fprintf(stderr, "%s", msg);
}

static int create_server_socket(int port) {
    // Creating a server socket
    // AF_INET for IPv4 (AF_INET6 for IPv6 or dual-stack sockets)
    // SOCK_STREAM for TCP
    // protocol set to 0 for auto
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

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
        htonl(INADDR_LOOPBACK); // htonl converts the address to big-endian
                                // (Host to Network Long)
                                // host is the CPU endian (small-endian)
                                // Network is the big-endian.
                                // INADDR_LOOPBACK defines the address 127.0.0.1

    if (bind(
            server_fd,
            (const sockaddr *)&address, // converts IPv4 address pointer into
                                        // older generic socket API pointer type
            sizeof(address)) == -1) {
        close(server_fd);
        throw std::runtime_error("Failed to bind server socket");
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        close(server_fd);
        throw std::runtime_error("Failed to listen on server socket!");
    }
    return server_fd;
}

int32_t read_full(int fd, char* buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);

        if (rv <= 0) {
            return -1; // error
        }

        assert((size_t) rv <= n);
        n -= (size_t) rv; // Update the number of remaining bytes to be read
        buf += rv; // add the read value to the buffer
    }

    return 0;
}

int32_t write_full(int fd, char* buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);

        if (rv <= 0) {
            return -1; // error
        }

        assert((size_t) rv <= n);
        n -= (size_t) rv; // Update the number of remaining bytes to be read
        buf += rv; // add the read value to the buffer
    }

    return 0;
}

const size_t k_max_msg = 4096;

int32_t one_request(int connfd) {
    // inlcuding a 4 bytes header
    char rbuf[4 + k_max_msg];
    errno = 0; // set to 0 manually as it is not reset after a successful syscall
    int32_t err = read_full(connfd, rbuf, 4);

    if (err) {
        error_msg(errno == 0 ? "EOF" : "Error in read() function");
        return err;
    }
    uint32_t len = 0;
    memcpy(&len, rbuf, 4); // assuming little endian here

    if (len > k_max_msg) {
        error_msg("Message too long");
        return -1;
    }

    // read request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        error_msg("Error in read()");
        return err;
    }

    // Do something with the message
    printf("Client says: %.*s\n", len, &rbuf[4]);

    // reply using same protocol
    const char reply[] = "world";

    char wbuf[4 + sizeof(reply)];
    len = (uint32_t) strlen(reply);

    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);

    return write_full(connfd, wbuf, 4 + len);
}

static void run_server(int server_fd, int port) {
    printf("Listening on port %d\n", port);

    while (true) {
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);

        int client_fd =
            accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);

        if (client_fd < 0) {
            fprintf(stderr, "Failed to accept client\n");
            continue; // we do not want to continue using this client socket
        }

        printf("Client Connected!\n");

        while (true) {
            int32_t err = one_request(client_fd);
            if (err) {
                break;
            }
        }

        close(client_fd);
    }
}

int main() {
    constexpr int port = 6379;
    int server_fd = -1;

    try {
        printf("MemVault is starting...\n");

        server_fd = create_server_socket(port);
        printf("Server socket successfully initialised and bound to port %d\n", port);

        run_server(server_fd, port);
    } catch (const std::exception &error) {
        if (server_fd != -1) {
            close(server_fd);
        }
        fprintf(stderr, "Server error: %s\n", error.what());
        return 1;
    }

    close(server_fd);
    return 0;
}
