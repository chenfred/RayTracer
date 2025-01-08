#include "thread/thread_pool.hpp"
#include <thread>

ThreadPool::ThreadPool(size_t thread_count) : alive{true}, num_pending_task{0} {
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

void ThreadPool::wait() const {
    while (num_pending_task != 0) {
        std::this_thread::yield();
    }
}

void ThreadPool::addTask(Task *task) {
    std::lock_guard<std::mutex> guard(lock);
    num_pending_task++;
    tasks.push_back(task);
}

Task *ThreadPool::getTask() {
    std::lock_guard<std::mutex> guard(lock);
    if (tasks.empty()) {
        return nullptr;
    }
    Task *task = tasks.front();
    tasks.pop_front();
    return task;
}

void ThreadPool::WorkerThread(int worker_id, ThreadPool *master) {
    while (master->alive) {
        if (master->num_pending_task == 0) {
            std::this_thread::yield();
        }

        Task *task = master->getTask();
        if (task != nullptr) {
            task->run();
            delete task;
            master->num_pending_task--;
        } else {
            std::this_thread::yield();
        }
    }
}