#include "thread_pool.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <thread>


ThreadPool::ThreadPool(size_t thread_count) {
    if (thread_count == 0) {
        thread_count = std::thread::hardware_concurrency();
    }
    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back(std::thread(ThreadPool::WorkerThread, i, this));
    }
}

ThreadPool::~ThreadPool() {
    for (auto &thread : threads) {
        thread.join();
    }
    threads.clear();
}

void ThreadPool::WorkerThread(int id, ThreadPool *master) {
    std::cout << fmt::format("Thread {} started", id) << std::endl;
}