#include "thread/thread_pool.hpp"
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
    if (thread_count == 0) {
        thread_count = std::thread::hardware_concurrency();
    }
    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back(std::thread(ThreadPool::WorkerThread, i, this));
    }
}

ThreadPool::~ThreadPool() {
    wait();
    alive = false;
    for (auto &thread : threads) {
        thread.join();
    }
    threads.clear();
}

void ThreadPool::parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda) {
    double divider = std::sqrt(threads.size()); // 把width*height切分成小块的chunk_width*chunk*height，均匀地分配给池子里的线程
    size_t chunk_width = std::ceil(static_cast<double>(width) / divider);
    size_t chunk_height = std::ceil(static_cast<double>(height) / divider);

    for (auto x = 0; x < width; x += chunk_width) {
        // 最后一块可能比较小
        auto cur_chunk_width = std::min(chunk_width, width - x);
        if (cur_chunk_width <= 0)
            break;
        for (auto y = 0; y < height; y += chunk_height) {
            auto cur_chunk_height = std::min(chunk_height, height - y);
            if (cur_chunk_height <= 0)
                break;

            addTask(new ParallelForTask(x, y, cur_chunk_width, cur_chunk_height, lambda));
        }
    }
}

void ThreadPool::wait() const {
    while (numPendingTasks != 0) {
        std::this_thread::yield();
    }
}

void ThreadPool::addTask(Task *task) {
    Guard guard(spinLock); // 使用 Guard 类管理 SpinLock
    numPendingTasks++;
    tasks.push_back(task);
}

Task *ThreadPool::getTask() {
    Guard guard(spinLock); // 使用 Guard 类管理 SpinLock
    if (tasks.empty()) {
        return nullptr;
    }
    Task *task = tasks.front();
    tasks.pop_front();
    return task;
}

void ThreadPool::WorkerThread(int worker_id, ThreadPool *master) {
    while (master->alive) {
        if (master->numPendingTasks == 0) {
            std::this_thread::yield();
        }

        Task *task = master->getTask();
        if (task != nullptr) {
            task->run();
            delete task;
            master->numPendingTasks--;
        } else {
            std::this_thread::yield();
        }
    }
}