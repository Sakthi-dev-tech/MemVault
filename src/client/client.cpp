#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include "../functions/functions.cpp"

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(6379);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("connect");
    }

    // multiple requests
    int32_t err = query(fd, "hello1");
    if (err) {
        return trigger_done(fd);
    }

    err = query(fd, "hello2");
    if (err) {
        return trigger_done(fd);
    }

    err = query(fd, "unknown command");
    if (err) {
        return trigger_done(fd);
    }
}
