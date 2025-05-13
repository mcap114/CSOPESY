#pragma once
#include <string>
#include <unordered_map>
#include <functional>

class CLI {
public:
    CLI();                          
    void run();                     

private:
    // Command map
    std::unordered_map<std::string, std::function<void()>> commands;

    // Helper methods
    void printHeader();           // Former printHeader()
    void clearScreen();             // Former clearScreen()

    void handleInitialize();
    void handleScreen();
    void handleSchedulerTest();
    void handleSchedulerStop();
    void handleReportUtil();
    void handleExit();

};