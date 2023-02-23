#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <utility>
#include <queue>
#include <future>
#include <mutex>
#include <atomic>

class threadPool {
public:
    threadPool(uint32_t numOfThreads);

    threadPool(const threadPool &) = delete;

    threadPool(threadPool &&) = delete;

    threadPool &operator=(const threadPool &) = delete;

    threadPool &operator=(threadPool &&) = delete;

    ~threadPool();

    template<class F, class... Args>
    void addTask(const F &func, Args &&... args) {
        std::lock_guard<std::mutex> queueLock(queueMtx);
        queue.emplace(std::async(std::launch::deferred, func, args...));
        queueCV.notify_one();
    }

    void wait();

private:
    void run();

    std::queue<std::future<void>> queue;
    std::mutex queueMtx;
    std::condition_variable queueCV;

    std::condition_variable taskWaiter;

    std::vector<std::thread> threads;

    std::atomic<bool> enough{false};
};

#endif //THREADPOOL_H
