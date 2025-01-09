#pragma once

#include <atomic>

class SpinLock {
public:
    void acquire();
    void release();

private:
    std::atomic_flag flag{};
};

class Guard {
public:
    Guard(SpinLock &_spinlock);
    ~Guard();

private:
    SpinLock &spinLock;
};