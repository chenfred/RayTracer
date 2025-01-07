#pragma once

#include <thread>
#include <vector>

class Task {
  public:
    virtual void run() = 0;
};

class ThreadPool {
  public:
    ThreadPool(size_t thread_count = 0);
    ~ThreadPool();

    static void WorkerThread(int id, ThreadPool *master);

  private:
    std::vector<std::thread> threads;
};