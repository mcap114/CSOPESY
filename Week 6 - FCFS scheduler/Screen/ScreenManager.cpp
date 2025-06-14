#include "ScreenManager.h"

void ScreenManager::createOrFocusScreen(const std::string& name, bool is_running) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    auto it = screens_.find(name);
    if (it != screens_.end()) {
        it->second->setRunning(is_running);  
    }
    else {
        screens_[name] = std::make_unique<Screen>(name, is_running);
    }
}

void ScreenManager::removeScreen(const std::string& name) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    screens_.erase(name);
}

bool ScreenManager::screenExists(const std::string& name) const {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    return screens_.find(name) != screens_.end();
}

std::string ScreenManager::renderScreen(const std::string& name) const {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    auto it = screens_.find(name);
    return (it != screens_.end()) ? it->second->render() : "Screen not found";
}

void ScreenManager::setRunning(const std::string& name, bool running) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    if (auto it = screens_.find(name); it != screens_.end()) {
        it->second->setRunning(running);  
    }
}

void ScreenManager::addProcess(const std::string& screen_name,
    const ProcessInfo& process) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    if (auto it = screens_.find(screen_name); it != screens_.end()) {
        it->second->addProcess(process);
    }
}

void ScreenManager::updateProcess(const std::string& screen_name, const std::string& process_name,
    const std::string& status, int core, const std::string& progress) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    if (auto it = screens_.find(screen_name); it != screens_.end()) {
        it->second->updateProcess(process_name, status, core, progress);
    }
}

void ScreenManager::removeProcess(const std::string& screen_name, const std::string& process_name) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    if (auto it = screens_.find(screen_name); it != screens_.end()) {
        it->second->removeProcess(process_name);
    }
}

std::string ScreenManager::listScreens() const {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    std::ostringstream oss;

    oss << "╭──────────────────────────────────────╮\n"
        << "│ ACTIVE PROCESSES (screen -r NAME)    │\n"
        << "├──────────────────────────────────────┤\n";

    if (screens_.empty()) {
        oss << "│ No active processes                 │\n";
    }
    else {
        for (const auto& [name, screen] : screens_) {
            std::string status = screen->isRunning() ? "RUNNING" : "FINISHED";
            oss << "║ " << std::left << std::setw(26) << name
                << " │ " << std::setw(15) << "FINISHED" << " ║\n";
        }

    }

    oss << "╰──────────────────────────────────────╯\n";
    return oss.str();
}