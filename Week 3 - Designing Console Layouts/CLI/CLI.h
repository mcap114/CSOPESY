#pragma once
#include <string>
#include <unordered_map>
#include <functional>

class CLI {
public:
    CLI();                          
    void run();     

    //state tracking
    //flag to signal which screen to draw
    enum class AppState { MAIN_MENU, IN_SCREEN };

private:

    //state tracking values 
    AppState current_state_;
    std::string active_screen_name_;

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