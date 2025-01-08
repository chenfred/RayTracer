#pragma once

#include <chrono>
#include <string>

class Timer{
  public:
    Timer(const std::string& _name);
    ~Timer();

    void conclude();

  private:
    bool alive;
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
};