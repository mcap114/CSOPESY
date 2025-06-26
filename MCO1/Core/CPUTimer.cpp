#include "CPUTimer.h"

CPUTimer& CPUTimer::getInstance() {
    static CPUTimer instance;
    return instance;
}

void CPUTimer::tick() {
    ++cpuCycles;
}

unsigned long CPUTimer::getTicks() const {
    return cpuCycles.load();
}