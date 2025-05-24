#include "ScreenManager.h"

void ScreenManager::createOrFocusScreen(const std::string& name, bool is_running) {
    auto it = screens_.find(name);
    if (it != screens_.end()) {
        it->second->setRunning(is_running);
    }
    else {
        screens_[name] = std::make_unique<Screen>(name, is_running);
    }
}

void ScreenManager::removeScreen(const std::string& name) {
    screens_.erase(name);
}

bool ScreenManager::screenExists(const std::string& name) const {
    return screens_.find(name) != screens_.end();
}

std::string ScreenManager::renderScreen(const std::string& name) const {
    auto it = screens_.find(name);
    return (it != screens_.end()) ? it->second->render() : "Screen not found";
}

void ScreenManager::setRunning(const std::string& name, bool running) {
    if (auto it = screens_.find(name); it != screens_.end()) {
        it->second->setRunning(running);
    }
}