#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Screen.h"
#include "Config.h"

class ScreenManager {
public:
    void createOrFocusScreen(const std::string& name, bool is_running);
    std::string renderScreen(const std::string& name) const;
    std::vector<std::string> listScreens() const;

    void tickAll();  // Used for global ticking
    void tickScreen(const std::string& name);         //tick a specific screen
    bool isProcessDone(const std::string& name) const; //check if a process is finished

    bool screenExists(const std::string& name) const;
    void setConfig(const Config& config);

private:
    std::unordered_map<std::string, Screen> screens_;
    Config config_;
};
