#include "thread/spin_lock.hpp"

#include <atomic>
#include <thread>

void SpinLock::acquire() {
    while (flag.test_and_set(std::memory_order_acquire)) {
        // 自旋等待锁释放
        std::this_thread::yield(); //不那么aggressive的自旋锁：旋不到就放弃，让kernel来决定
    }
}

void SpinLock::release() {
    flag.clear(std::memory_order_release);
}

// 添加 Guard 类的构造函数
Guard::Guard(SpinLock &_spinlock) : spinLock(_spinlock) {
    spinLock.acquire();
}

// 添加 Guard 类的析构函数
Guard::~Guard() {
    spinLock.release();
}

