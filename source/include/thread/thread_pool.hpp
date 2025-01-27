#pragma once

#include <atomic>
#include <deque>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

class Task {
public:
    Task() = default;
    virtual ~Task() = default;

    virtual void run() = 0;
};

class ParallelForTask : public Task {
public:
    ParallelForTask(size_t _x, size_t _y, size_t _chunk_width, size_t _chunk_height,
                    const std::function<void(size_t, size_t)> &_lambda)
        : x{_x}, y{_y}, chunkWidth{_chunk_width}, chunkHeight{_chunk_height}, lambda{_lambda} {}

    void run() override;

private:
    size_t x, y, chunkWidth, chunkHeight;
    std::function<void(size_t, size_t)> lambda;
};

class ThreadPool {
public:
    ThreadPool() = delete;
    ThreadPool(size_t thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();

    void parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &f, bool complexTask = true);
    void serialFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &f);
    void wait();

    void addTask(Task *task);
    Task *getTask();

    static void WorkerThread(int worker_id, ThreadPool *master);

private:
    std::atomic<bool> alive;
    std::atomic<size_t> numPendingTasks;
    std::vector<std::thread> threads;
    std::deque<Task *> tasks;

    std::mutex queueMutex;
    std::condition_variable queueCondition;
};
