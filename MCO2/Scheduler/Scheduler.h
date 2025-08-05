#pragma once
#include <string>
#include <queue>
#include <memory>
#include "../Screen/ScreenManager.h"

class Scheduler {
public:
    Scheduler(ScreenManager& sm) : screen_manager_(sm) {}
    virtual ~Scheduler() = default;

    virtual void addProcess(const std::string& name) = 0;
    virtual void tick() = 0;

protected:
    ScreenManager& screen_manager_;
};