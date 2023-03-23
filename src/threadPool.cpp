#include "threadPool.h"

threadPool::threadPool(uint32_t numOfThreads) {
    threads.reserve(numOfThreads);

    for (uint32_t i = 0; i < numOfThreads; ++i)
        threads.emplace_back(&threadPool::run, this);
}

threadPool::~threadPool() {
    wait();
    enough = true;
    queueCV.notify_all();
    for (auto &it: threads)
        it.join();
}

void threadPool::run() {
    while (!enough) {
        std::unique_lock<std::mutex> lock(queueMtx);
        queueCV.wait(lock, [this]() { return !queue.empty() || enough; });

        if (!queue.empty()) {
            auto task = std::move(queue.front());
            queue.pop();
            lock.unlock();

            task.get();
            taskWaiter.notify_all();
        }
    }
}

void threadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMtx);
    taskWaiter.wait(lock, [this](){ return queue.empty(); });
}
