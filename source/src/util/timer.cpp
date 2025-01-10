#include "util/timer.hpp"
#include <iostream>


Timer::Timer(const std::string& _name) : alive(true), name(_name), start(std::chrono::high_resolution_clock::now()) {
}


Timer::~Timer() {
    conclude();
}


void Timer::conclude() {
    if (alive) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "Timer " << name << " concluded in " << duration.count() << " milliseconds." << std::endl;
        alive = false;
    }
}

