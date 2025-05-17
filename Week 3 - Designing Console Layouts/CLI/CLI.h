#pragma once
#include <string>
#include <unordered_map>
#include <functional>

class CLI {
public:
    CLI();                          
    void run();     

    enum class AppState { MAIN_MENU, IN_SCREEN };
    AppState current_state_;
    std::string active_screen_name_;

private:
    // Command map
    std::unordered_map<std::string, std::function<void()>> commands;

    // Helper methods
    void printHeader();           // Former printHeader()
    void clearScreen();             // Former clearScreen()
    void returnToMainMenu();


    void handleInitialize();
    void handleScreen(const std::string& args);
    void handleSchedulerTest();
    void handleSchedulerStop();
    void handleReportUtil();
    void handleExit();

};