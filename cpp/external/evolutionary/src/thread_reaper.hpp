#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace elizaos::evolutionary_detail {

class ThreadReaper final {
public:
    static ThreadReaper& instance() {
        static ThreadReaper reaper;
        return reaper;
    }

    ThreadReaper(const ThreadReaper&) = delete;
    ThreadReaper& operator=(const ThreadReaper&) = delete;

    void adopt(std::thread thread) {
        if (!thread.joinable()) return;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            threads_.push_back(std::move(thread));
        }
        condition_.notify_one();
    }

private:
    ThreadReaper() : worker_([this] { run(); }) {}

    ~ThreadReaper() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }
        condition_.notify_all();
        if (worker_.joinable()) worker_.join();
    }

    void run() noexcept {
        for (;;) {
            std::thread thread;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this] { return stopping_ || !threads_.empty(); });
                if (threads_.empty()) {
                    if (stopping_) return;
                    continue;
                }
                thread = std::move(threads_.back());
                threads_.pop_back();
            }
            if (thread.joinable()) thread.join();
        }
    }

    std::mutex mutex_;
    std::condition_variable condition_;
    std::vector<std::thread> threads_;
    bool stopping_ = false;
    std::thread worker_;
};

inline void joinWithoutDetach(std::thread thread) {
    ThreadReaper::instance().adopt(std::move(thread));
}

}  // namespace elizaos::evolutionary_detail
