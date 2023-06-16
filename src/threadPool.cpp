#include "threadPool.h"

threadPool::threadPool(uint32_t numOfThreads) {
    threads.reserve(numOfThreads);

    for (uint32_t i = 0; i < numOfThreads; ++i)
        threads.emplace_back(&threadPool::run, this);
}

threadPool::~threadPool() {
    enough = true;
    for (auto &it: threads) {
        queueCV.notify_all();
        it.join();
    }
}

void threadPool::run() {
    while (!enough) {
        std::unique_lock<std::mutex> lock(queueMtx);
        queueCV.wait(lock, [this]() { return !queue.empty() || enough; });

        if (!queue.empty()) {
            auto task = std::move(queue.front());
            queue.pop();
            lock.unlock();

            task.first.get();

            std::lock_guard<std::mutex> lock(taskMtx);
            completedTasks.insert(task.second);

            taskWaiter.notify_all();
        }
    }
}

void threadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMtx);
    taskWaiter.wait(lock, [this](){
        std::lock_guard<std::mutex> lock(taskMtx);
        return queue.empty() && lastTask == completedTasks.size();
    });
}
