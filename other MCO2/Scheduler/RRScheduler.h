#pragma once
#include "Scheduler.h"
#include <unordered_map>

class RRScheduler : public Scheduler {
public:
    RRScheduler(ScreenManager& sm, int quantum);
    void addProcess(const std::string& name) override;
    void tick() override;

private:
    int quantum_;
    std::queue<std::string> ready_queue_;
    std::unordered_map<std::string, int> time_remaining_;

    void tickProcess(const std::string& name);
    bool isProcessFinished(const std::string& name);
};