#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>

class CLI {
public:
    CLI();                          
    void run();                     

private:
    // Screen metadata
    struct Screen {
        std::string name;
        int currentLine;
        int totalLines;
        std::string timestamp;
    };

    // Command map
    std::unordered_map<std::string, std::function<void()>> commands;

    // Screens storage
    std::unordered_map<std::string, Screen> screens;

    // Helper methods
    void printHeader();  
    void clearConsole(); 
    void clearScreen();         
                
    std::string getCurrentTimestamp();

    // Command handlers
    void handleInitialize();
    void handleScreen();
    void handleSchedulerTest();
    void handleSchedulerStop();
    void handleReportUtil();
    void handleExit();

    // Screen behavior
    void createOrResumeScreen(const std::string& name, bool resume);
    void runScreenConsole(const Screen& screen);
};
