#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "ScreenManager.h"
#include "FCFSScheduler.h"

class CLI {
public:
    CLI();
    ~CLI();
    void run();

    enum class AppState { MAIN_MENU, IN_SCREEN };

private:
    bool is_initialized_ = false;

    // State tracking
    int next_process_id_ = 1;
    AppState current_state_;
    std::string active_screen_name_;
    ScreenManager screen_manager_;
    std::unique_ptr<FCFSScheduler> scheduler_;

    // Command map with argument support
    std::unordered_map<std::string, std::function<void(const std::string&)>> commands;

    // Core methods
    void printHeader(bool show_prompt = true);
    void clearScreen();
    void returnToMainMenu();

    // Command handlers (all with args)
    void handleInitialize(const std::string& args);
    void handleScreen(const std::string& args);
    void handleSchedulerTest(const std::string& args);
    void handleSchedulerStop(const std::string& args);
    void handleReportUtil(const std::string& args);
    void handleExit(const std::string& args);

    // Helper functions
    void createProcessScreen(const std::string& processName, int totalPrints);
    bool parseScreenArgs(const std::string& args, char& mode, std::string& name);
};