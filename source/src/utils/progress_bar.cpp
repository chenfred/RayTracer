#include "utils/progress_bar.hpp"

#include <format>
#include <iostream>

void ProgressBar::update(double percent) {
    int pos = static_cast<int>(width * percent);
    for (int i = 0; i < width; ++i) {
        if (i < pos) {
            bar[i] = '=';
        } else if (i == pos) {
            bar[i] = '>';
        } else {
            bar[i] = ' ';
        }
    }
    std::cout << std::format("{}: [{}] {:.2f}%\r", name.c_str(), bar.c_str(), percent * 100);
    // std::cout << name << ": [" << bar << "] " << percent * 100 << "%\r";
    std::cout.flush();
}

void ProgressBar::done() {
    // 清除当前行
    std::cout << "\r";
    // 填充进度条为100%
    for (int i = 0; i < width; ++i) {
        bar[i] = '=';
    }
    std::cout << std::format("{}: [{}] {:.2f}%\n", name.c_str(), bar.c_str(), 100.0);
    std::cout.flush();
}
