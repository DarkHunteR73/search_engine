#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <utility>
#include <queue>
#include <future>
#include <mutex>
#include <atomic>
#include <unordered_set>

class threadPool {
public:
    explicit threadPool(uint32_t numOfThreads);

    ~threadPool();

    threadPool(const threadPool &) = delete;

    threadPool(threadPool &&) = delete;

    threadPool &operator=(const threadPool &) = delete;

    threadPool &operator=(threadPool &&) = delete;

    template<class F, class... Args>
    int32_t addTask(const F &func, Args &&... args) {
        int32_t taskId = lastTask++;

        std::lock_guard<std::mutex> queueLock(queueMtx);
        queue.emplace(std::async(std::launch::deferred, func, args...), taskId);
        queueCV.notify_one();

        return taskId;
    }

    void wait();

private:
    void run();

    std::queue<std::pair<std::future<void>, uint32_t>> queue;
    std::mutex queueMtx;
    std::condition_variable queueCV;

    std::vector<std::thread> threads;

    std::atomic<bool> enough{false};

    std::unordered_set<uint32_t> completedTasks;
    std::atomic<uint32_t> lastTask{0};
    std::mutex taskMtx;
    std::condition_variable taskWaiter;
};

#endif //THREADPOOL_H
