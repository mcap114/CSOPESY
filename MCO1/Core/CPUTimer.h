#pragma once
#include <atomic>

class CPUTimer {
public:
    static CPUTimer& getInstance();
    void tick();
    unsigned long getTicks() const;

private:
    CPUTimer() = default;
    std::atomic<unsigned long> cpuCycles{ 0 };
};