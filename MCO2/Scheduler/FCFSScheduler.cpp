#include "FCFSScheduler.h"

FCFScheduler::FCFScheduler(ScreenManager& sm) : Scheduler(sm) {}

void FCFScheduler::addProcess(const std::string& name) {
    ready_queue_.push(name);
}

void FCFScheduler::tick() {
    if (ready_queue_.empty()) return;

    const std::string& current = ready_queue_.front();
    tickProcess(current);

    if (isProcessFinished(current)) {
        ready_queue_.pop();
    }
}

void FCFScheduler::tickProcess(const std::string& name) {
    screen_manager_.tickScreen(name);
}

bool FCFScheduler::isProcessFinished(const std::string& name) {
    return screen_manager_.isProcessDone(name);
}