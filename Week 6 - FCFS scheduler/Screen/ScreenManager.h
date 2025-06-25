#pragma once
#include "Screen.h"
#include <unordered_map>
#include <memory>
#include <mutex>

struct ProcessDisplayInfo {
    std::string name;
    std::string status;  // "RUNNING" or "COMPLETED"
    int core;
    std::string creation_time;
};


class ScreenManager {
public:
    void createOrFocusScreen(const std::string& name, bool is_running);
    void removeScreen(const std::string& name);
    bool screenExists(const std::string& name) const;
    std::string renderScreen(const std::string& name) const;
    void setRunning(const std::string& name, bool running);

    // Process management
    void addProcess(const std::string& screen_name, const ProcessInfo& process);
    void updateProcess(const std::string& screen_name, const std::string& process_name,
        const std::string& status, int core, const std::string& progress);
    void removeProcess(const std::string& screen_name, const std::string& process_name);

    // Listing functionality
    std::string listScreens() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Screen>> screens_;
    std::vector<ProcessDisplayInfo> completed_processes_; // tracks COMPLETED
    mutable std::mutex screen_mutex_;
};
