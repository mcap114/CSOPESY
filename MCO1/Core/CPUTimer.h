#pragma once
#include <atomic>

class CPUTimer {
public:
    static void tick() {
        ++tickCount;
    }

    static unsigned long getTicks() {
        return tickCount.load();
    }

private:
    static std::atomic<unsigned long> tickCount;
};
