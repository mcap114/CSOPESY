#include "ScreenManager.h"
#include <iostream>

void ScreenManager::createOrFocusScreen(const std::string& name, bool is_running) {
    if (screens_.find(name) == screens_.end()) {
        screens_.emplace(name, Screen(name, is_running));
        std::cout << "Created screen: " << name << "\n";
    }
    else {
        std::cout << "Focusing on existing screen: " << name << "\n";
    }
}

std::string ScreenManager::renderScreen(const std::string& name) const {
    auto it = screens_.find(name);
    if (it != screens_.end()) {
        return it->second.render();
    }
    return "Screen not found.\n";
}

std::vector<std::string> ScreenManager::listScreens() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : screens_) {
        names.push_back(name);
    }
    return names;
}

bool ScreenManager::screenExists(const std::string& name) const {
    return screens_.find(name) != screens_.end();
}


void ScreenManager::tickAll() {
    for (auto& [name, screen] : screens_) {
        if (screen.isRunning()) {
            screen.tick();
        }
    }
}

void ScreenManager::setConfig(const Config& config) {
    config_ = config;
    // will be useful when screens need config-driven values
}

void ScreenManager::tickScreen(const std::string& name) {
    auto it = screens_.find(name);
    if (it != screens_.end() && it->second.isRunning()) {
        it->second.tick();
    }
}

bool ScreenManager::isProcessDone(const std::string& name) const {
    auto it = screens_.find(name);
    if (it != screens_.end()) {
        return !it->second.isRunning();  // not running = done
    }
    return true;  // if not found, treat as finished
}
