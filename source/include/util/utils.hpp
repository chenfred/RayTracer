#pragma once

template <typename T>
inline bool in_range(T value, T low, T high) {
    return value >= low && value <= high;
}
