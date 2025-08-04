#include "CPUTimer.h"

void CPUTimer::start() {
    ticks = 0;
}

void CPUTimer::tick() {
    ++ticks;
}

unsigned long long CPUTimer::getTicks() const {
    return ticks;
}
