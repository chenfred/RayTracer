#pragma once

#include <string>

class ProgressBar {
  public:
    ProgressBar(const std::string& _name = "Progress", int _width = 50) : name(_name), width(_width), bar(width, ' ') {}
    void update(double percent);
    void done();

  private:
    int width;
    std::string name, bar;
};
