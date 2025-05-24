#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "ScreenManager.h"

class CLI {
public:
    CLI();
    void run();

    enum class AppState { MAIN_MENU, IN_SCREEN };

private:
    // State tracking
    AppState current_state_;
    std::string active_screen_name_;
    ScreenManager screen_manager_;

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
    bool parseScreenArgs(const std::string& args, char& mode, std::string& name);
};