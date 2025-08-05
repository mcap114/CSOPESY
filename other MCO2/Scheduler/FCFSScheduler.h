#pragma once
#include "Scheduler.h"

class FCFScheduler : public Scheduler {
public:
    FCFScheduler(ScreenManager& sm);
    void addProcess(const std::string& name) override;
    void tick() override;

private:
    std::queue<std::string> ready_queue_;

    void tickProcess(const std::string& name);
    bool isProcessFinished(const std::string& name);
};
