#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>  
#include <ctime>    
#include <vector>

class Process {
public:
    Process(const std::string& name, int totalPrints);
    void executePrint(int coreId);
    void step();
    bool isCompleted() const;
    const std::string& getName() const;
    int getCoreId() const;

    void handleSleep(unsigned long ticks);
    void handleForLoop(const std::vector<std::string>& body, int repeat);

private:
    std::string name;
    int totalPrints;
    int printsCompleted = 0;
    int assignedCore = -1;
    std::ofstream logFile;

    // Sleep state
    bool sleeping = false;
    unsigned long sleepStartTick = 0;
    unsigned long sleepDuration = 0;

    // FOR loop state
    bool inForLoop = false;
    int forLoopCounter = 0;
    int forLoopLimit = 0;
    std::vector<std::string> loopBody;

    std::string getCurrentTimestamp();
};
