#include "utils/timed.h"
#include <iostream>

Timed::Timed() {
    beginTime = std::chrono::steady_clock::now();
}

void Timed::printElapsedTime() {
    auto currTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(
            currTime - beginTime);
    std::cout << "[Timed] " << timeSpan.count() << " seconds passed." << std::endl;
}
