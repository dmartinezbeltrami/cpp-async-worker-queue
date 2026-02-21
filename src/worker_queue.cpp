#include "worker_queue.h"

#include <utility>

template <typename T>
WorkerQueue<T>::WorkerQueue(std::size_t max_queue_size)
    : max_queue_size_(max_queue_size) {}

template <typename T>
WorkerQueue<T>::~WorkerQueue() {
    stop();
}

template <typename T>
void WorkerQueue<T>::start() {
    if (running_) {
        return;
    }

    running_ = true;
    worker_ = std::thread(&WorkerQueue::worker_loop, this);
}

template <typename T>
void WorkerQueue<T>::stop() {
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

template <typename T>
bool WorkerQueue<T>::post(T item) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!running_ || queue_.size() >= max_queue_size_) {
        return false;
    }

    queue_.push_back(std::move(item));
    cv_.notify_one();
    return true;
}

template <typename T>
bool WorkerQueue<T>::is_running() const {
    return running_;
}

template <typename T>
void WorkerQueue<T>::worker_loop() {
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

        // Process item outside the lock
        item();
    }
}