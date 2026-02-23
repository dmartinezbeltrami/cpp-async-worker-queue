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
- Suitable for embedded Linux / Windows environments  

---

## Typical use cases

- Alert or event processing  
- Logging  
- Asynchronous uploads  
- Background housekeeping tasks  
- Any work that must not block a real-time loop  

---

## Project structure

```text
cpp-async-worker-queue/
├── include/
│   └── worker_queue.h
├── examples/
│   └── simple_example.cpp
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

---

## Usage

Basic example using `WorkerQueue` with `std::function<void()>`:

```cpp
#include "worker_queue.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

int main() {
    // Create a worker queue that can hold up to 8 pending tasks
    WorkerQueue<std::function<void()>> queue(8);

    std::cout << "Starting worker queue...\n";
    queue.start();

    // Post a few simple tasks
    for (int i = 0; i < 5; ++i) {
        bool ok = queue.post([i] {
            std::cout << "Processing task " << i << " on thread "
                      << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });

        if (!ok) {
            std::cout << "Queue full, dropping task " << i << "\n";
        }
    }

    // Give the worker some time to process the tasks (for demo purposes)
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Stopping worker queue...\n";
    queue.stop();
    std::cout << "Done.\n";

    return 0;
}
```
To build with CMake (on Windows, Linux, or macOS):
```Bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```
Then run:
```Bash
# Windows (MSVC)
.\Debug\simple_example.exe

# Linux / macOS (single-config generators)
./simple_example
```
