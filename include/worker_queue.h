#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

template <typename T>
class WorkerQueue {
public:
    explicit WorkerQueue(std::size_t max_queue_size = 100);
    ~WorkerQueue();

    // Starts the background worker thread
    void start();

    // Signals the worker to stop and waits for it to finish
    void stop();

    // Attempts to enqueue an item.
    // Returns false if the queue is full or the worker is not running.
    bool post(T item);

    // Returns whether the worker thread is running
    bool is_running() const;

private:
    void worker_loop();

    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<T> queue_;

    std::atomic<bool> running_{false};
    std::size_t max_queue_size_;
};