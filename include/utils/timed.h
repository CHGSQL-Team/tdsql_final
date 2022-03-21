#pragma once
#include <chrono>

class Timed {
private:
    std::chrono::steady_clock::time_point beginTime;
public:
    Timed();
    void printElapsedTime();
};