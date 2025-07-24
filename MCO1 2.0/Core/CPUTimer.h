#pragma once

class CPUTimer {
public:
    void start();
    void tick(); // Simulate a CPU tick
    unsigned long long getTicks() const;
private:
    unsigned long long ticks = 0;
};
