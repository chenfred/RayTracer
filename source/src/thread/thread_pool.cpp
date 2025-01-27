#include "thread/thread_pool.hpp"

#include <cassert>
#include <cmath>
#include <thread>

void ParallelForTask::run() {
    for (auto i = 0; i < chunkWidth; ++i) {
        for (auto j = 0; j < chunkHeight; ++j) {
            lambda(x + i, y + j);
        }
    }
}

ThreadPool::ThreadPool(size_t thread_count) : alive{true}, numPendingTasks{0} {
    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back(std::thread(ThreadPool::WorkerThread, i, this));
    }
}

ThreadPool::~ThreadPool() {
    wait();
    alive = false;
    queueCondition.notify_all();
    for (auto &thread : threads) {
        thread.join();
    }
    threads.clear();
}

void ThreadPool::parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &f, bool complexTask) {
    if (threads.empty()) {
        serialFor(width, height, f);
        return;
    }

    double divider = std::sqrt(threads.size());
    if (complexTask) {
        divider *= 4;
    }

    size_t chunk_width = std::ceil(static_cast<double>(width) / divider);
    size_t chunk_height = std::ceil(static_cast<double>(height) / divider);

    for (size_t x = 0; x < width; x += chunk_width) {
        size_t cur_chunk_width = std::min(chunk_width, width - x);
        if (cur_chunk_width <= 0)
            break;
        for (size_t y = 0; y < height; y += chunk_height) {
            size_t cur_chunk_height = std::min(chunk_height, height - y);
            if (cur_chunk_height <= 0)
                break;

            addTask(new ParallelForTask(x, y, cur_chunk_width, cur_chunk_height, f));
        }
    }
}

void ThreadPool::serialFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &f) {
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            f(x, y);
        }
    }
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMutex);
    queueCondition.wait(lock, [this]() { return numPendingTasks == 0; });
}

void ThreadPool::addTask(Task *task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        numPendingTasks++;
        tasks.push_back(task);
    }
    queueCondition.notify_one();
}

Task *ThreadPool::getTask() {
    std::unique_lock<std::mutex> lock(queueMutex);
    queueCondition.wait(lock, [this]() { return !tasks.empty() || !alive; });
    if (!alive && tasks.empty()) {
        return nullptr;
    }

    Task *task = tasks.front();
    tasks.pop_front();
    return task;
}

void ThreadPool::WorkerThread(int worker_id, ThreadPool *master) {
    while (master->alive) {
        Task *task = master->getTask();
        if (task != nullptr) {
            task->run();
            delete task;
            {
                std::lock_guard<std::mutex> lock(master->queueMutex);
                master->numPendingTasks--;
                if (master->numPendingTasks == 0) {
                    master->queueCondition.notify_all();
                }
            }
        }
    }
}
