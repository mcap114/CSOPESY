#include "RRScheduler.h"

RRScheduler::RRScheduler(ScreenManager& sm, int quantum)
    : Scheduler(sm), quantum_(quantum) {
}

void RRScheduler::addProcess(const std::string& name) {
    ready_queue_.push(name);
    time_remaining_[name] = quantum_;
}

void RRScheduler::tick() {
    if (ready_queue_.empty()) return;

    std::string& current = ready_queue_.front();
    tickProcess(current);
    time_remaining_[current]--;

    if (isProcessFinished(current)) {
        ready_queue_.pop();
        time_remaining_.erase(current);
    }
    else if (time_remaining_[current] <= 0) {
        ready_queue_.push(current);
        ready_queue_.pop();
        time_remaining_[ready_queue_.front()] = quantum_;
    }
}

void RRScheduler::tickProcess(const std::string& name) {
    screen_manager_.tickScreen(name);
}

bool RRScheduler::isProcessFinished(const std::string& name) {
    return screen_manager_.isProcessDone(name);
}