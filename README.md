# MemVault
Redis but in C++

# Redis-Compatible In-Memory Datastore

A lightweight, Redis-compatible in-memory data structure store built entirely from scratch in modern C++. 

This project serves as an educational deep-dive into systems programming, specifically designed to explore high-performance asynchronous network programming, custom protocol parsing, disk persistence mechanisms, and master-replica replication.

## 🚀 Key Features

*   **RESP Compatibility:** Implements the Redis Serialization Protocol (RESP), allowing seamless interaction using the standard `redis-cli` and existing Redis client libraries.
*   **Asynchronous Networking:** Uses an event-driven, non-blocking I/O architecture (via `epoll`/`kqueue`) to efficiently manage thousands of concurrent client connections.
*   **Core Data Structures:** Supports essential Redis data types including Strings, Lists, Hashes, and Sets.
*   **Persistence Mechanisms:**
    *   **Snapshotting:** Point-in-time memory dumps (similar to Redis RDB).
    *   **Append-Only File (AOF):** Logs every write operation for robust data durability and crash recovery.
*   **Replication:** Basic leader-follower (master-replica) state synchronization.

## 🎯 Educational Goals & Explorations

This datastore was built to unpack the "magic" behind production-grade caching systems:
*   **Asynchronous Network Programming:** Understanding event loops, reactor patterns, and handling partial TCP reads/writes.
*   **Protocol Design:** Building a zero-copy (where possible) parser and serializer for streaming data over TCP.
*   **Memory Management & Concurrency:** Designing thread-safe, low-latency access patterns for in-memory data structures.
*   **Persistence & I/O:** Implementing background file I/O that doesn't block the main event loop during snapshots.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
