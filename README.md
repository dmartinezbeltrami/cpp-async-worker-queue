# cpp-async-worker-queue

A minimal C++11 implementation of a **bounded, single-threaded worker queue**
designed for real-time and embedded systems.

This project focuses on correctness, simplicity, and predictable behavior,
not maximum throughput.

---

## Why this exists

In real-time applications (video pipelines, embedded devices, edge AI systems),
blocking the main thread to perform background work can cause:

- frame drops
- latency spikes
- unstable behavior under load

A common pattern is to offload non-critical work to a background thread,
while **bounding memory usage and execution time**.

This repository demonstrates a small, production-oriented worker queue that:

- runs work on a dedicated thread
- enforces a maximum queue size (backpressure)
- starts and stops cleanly
- avoids hidden background activity on shutdown

---

## Features

- Single background worker thread
- Bounded queue (prevents unbounded memory growth)
- Thread-safe task submission
- Graceful start / stop semantics
- No external dependencies
- Suitable for embedded Linux environments

---

## Typical use cases

- Alert or event processing
- Logging
- Asynchronous uploads
- Background housekeeping tasks
- Any work that must not block a real-time loop

---

## Project structure
```
cpp-async-worker-queue/
├── include/
│ └── worker_queue.h
├── src/
│ └── worker_queue.cpp
├── examples/
│ └── simple_example.cpp
├── CMakeLists.txt
└── README.md
```
---

## Design goals

- **Explicit behavior** over clever abstractions
- **Predictable shutdown** (no detached threads)
- **Backpressure** instead of silently growing queues
- Easy to reason about and extend

This is not a framework — it is a small building block.
