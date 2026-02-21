#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>
#include <utility>

// WorkerQueue is a minimal bounded, single-threaded worker queue.
// It expects T to be a callable type (e.g. std::function<void()> or a lambda).
//
// Typical usage:
//   WorkerQueue<std::function<void()>> q(100);
//   q.start();
//   q.post([] { /* do some work */ });
//   q.stop();

template <typename T>
class WorkerQueue {
public:
    explicit WorkerQueue(std::size_t max_queue_size = 100)
        : max_queue_size_(max_queue_size) {}

    ~WorkerQueue() {
        stop();
    }

    // Starts the background worker thread.
    void start() {
        if (running_) {
            return;
        }

        running_ = true;
        worker_ = std::thread(&WorkerQueue::worker_loop, this);
    }

    // Signals the worker to stop and waits for it to finish.
    void stop() {
        if (!running_) {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }

        cv_.notify_all();

        if (worker_.joinable()) {
            worker_.join();
        }
    }

    // Attempts to enqueue an item.
    // Returns false if the queue is full or the worker is not running.
    bool post(T item) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!running_ || queue_.size() >= max_queue_size_) {
            return false;
        }

        queue_.push_back(std::move(item));
        cv_.notify_one();
        return true;
    }

    // Returns whether the worker thread is running.
    bool is_running() const {
        return running_;
    }

private:
    void worker_loop() {
        while (true) {
            T item;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                cv_.wait(lock, [this] {
                    return !queue_.empty() || !running_;
                });

                if (!running_ && queue_.empty()) {
                    break;
                }

                item = std::move(queue_.front());
                queue_.pop_front();
            }

            // Process the item outside the lock.
            // For this minimal implementation, T is expected to be callable.
            item();
        }
    }

    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<T> queue_;

    std::atomic<bool> running_{false};
    std::size_t max_queue_size_;
};