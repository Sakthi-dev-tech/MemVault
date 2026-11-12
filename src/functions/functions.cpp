#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include<assert.h>
#include <vector>

/**
 * ================================================
 * CONSTANTS
 * ================================================
 */

const size_t k_max_msg = 4096;

/**
 * ================================================
 * MISC FUNCTIONS
 * ================================================
 */

int trigger_done(int fd) {
    close(fd);
    return 0;
}

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

void error_msg(const char *msg) { fprintf(stderr, "%s", msg); }

void fd_set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1) {
        throw std::runtime_error("F_GETFL failed!");
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        throw std::runtime_error("F_SETFL failed!");
    };
}

// append to the back
void buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len) {
    buf.insert(buf.end(), data, data + len);
}

// remove from the front
void buf_consume(std::vector<uint8_t> &buf, size_t n) {
    buf.erase(buf.begin(), buf.begin() + n);
}

/**
 * ================================================
 * CORE FUNCTIONS
 * ================================================
 */

int32_t read_full(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or unexpected EOF
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

int32_t write_all(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t query(int fd, const char *text) {
    uint32_t len = (uint32_t)strlen(text);
    if (len > k_max_msg) {
        return -1;
    }

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4);  // assume little endian
    memcpy(&wbuf[4], text, len);
    if (int32_t err = write_all(fd, wbuf, 4 + len)) {
        return err;
    }

    // 4 bytes header
    char rbuf[4 + k_max_msg];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err) {
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }

    memcpy(&len, rbuf, 4);  // assume little endian
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // reply body
    err = read_full(fd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    // do something
    printf("server says: %.*s\n", len, &rbuf[4]);
    return 0;
}

