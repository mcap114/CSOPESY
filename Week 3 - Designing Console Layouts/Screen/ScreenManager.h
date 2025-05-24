#pragma once
#include "Screen.h"
#include <unordered_map>
#include <memory>

class ScreenManager {
public:
    void createOrFocusScreen(const std::string& name, bool is_running);
    void removeScreen(const std::string& name);
    bool screenExists(const std::string& name) const;
    std::string renderScreen(const std::string& name) const;
    void setRunning(const std::string& name, bool running);

private:
    std::unordered_map<std::string, std::unique_ptr<Screen>> screens_;
};