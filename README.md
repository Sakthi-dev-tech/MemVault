# MemVault

Redis, but in C++.

MemVault is an educational in-memory datastore built from scratch in modern
C++, with the goal of becoming compatible with Redis clients. The project
explores systems programming topics such as TCP networking, RESP protocol
parsing, event-driven I/O, persistence, and replication.

> [!NOTE]
> MemVault is in an early stage of development. It currently provides a small,
> blocking TCP server prototype rather than a complete datastore.

## Project Status

### Implemented

- [x] IPv4 TCP server bound to `127.0.0.1:6379`
- [x] Reusable server address through `SO_REUSEADDR`
- [x] Sequential client connection handling with a blocking accept loop
- [x] Basic `PING` support for RESP (`*1\r\n$4\r\nPING\r\n`) and inline requests
- [x] RESP simple-string `+PONG` response
- [x] RESP error response for unknown commands
- [x] Server socket cleanup through its destructor and on initialization errors
- [x] Small Python client for manually exercising supported responses

### In Progress

- [ ] General RESP request parsing instead of matching a hard-coded `PING`
- [ ] Multiple requests over a persistent client connection

### Planned

- [ ] Core string commands and an in-memory key-value store
- [ ] Additional data structures such as lists, hashes, and sets
- [ ] Non-blocking, event-driven networking with `epoll`/`kqueue`
- [ ] Correct handling of partial TCP reads and writes
- [ ] Snapshot persistence
- [ ] Append-only file persistence and crash recovery
- [ ] Leader-follower replication
- [ ] Automated tests and a documented build workflow

## Current Behavior

The server listens only on the local loopback interface. It accepts one request
per connection, responds to `PING` with `PONG`, returns an error for any other
input, and then closes the client connection.

## Educational Goals

- Understand event loops, reactor patterns, and asynchronous network I/O.
- Build a streaming RESP parser and serializer.
- Explore low-latency access patterns and memory management for in-memory data.
- Implement background persistence without blocking the main event loop.
- Explore state synchronization between leader and replica nodes.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for
details.
