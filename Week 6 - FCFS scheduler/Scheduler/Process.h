#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>  
#include <ctime>    

class Process {
public:
    Process(const std::string& name, int totalPrints);
    void executePrint(int coreId);
    bool isCompleted() const;
    const std::string& getName() const;
    int getCoreId() const;

private:
    std::string name;
    int totalPrints;
    int printsCompleted = 0;
    int assignedCore = -1;
    std::ofstream logFile;

    std::string getCurrentTimestamp();
};