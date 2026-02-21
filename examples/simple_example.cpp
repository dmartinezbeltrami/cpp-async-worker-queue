#include "worker_queue.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

int main() {
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

    // Give the worker some time to process
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Stopping worker queue...\n";
    queue.stop();
    std::cout << "Done.\n";

    return 0;
}