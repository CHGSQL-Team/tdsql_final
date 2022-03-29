#pragma once

#include <mutex>
#include <condition_variable>

class semaphore {
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 0; // Initialized as locked.

public:

    explicit semaphore(unsigned long count);

    void release();

    void acquire();

    bool try_acquire();
};