#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

class Task {
  public:
    Task() = default;
    virtual ~Task() = default;
    
    virtual void run() = 0;
};

class ThreadPool {
  public:
    ThreadPool(size_t thread_count = 0);
    ~ThreadPool();

    void wait() const;

    void addTask(Task *task);
    Task *getTask();

    static void WorkerThread(int worker_id, ThreadPool *master);

  private:
    std::atomic<bool> alive;
    std::atomic<unsigned int> num_pending_task;
    std::vector<std::thread> threads;
    std::list<Task *> tasks;
    std::mutex lock;
};