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
    bool isCompleted() const;
    const std::string& getName() const;
    int getCoreId() const;
    const std::vector<std::string>& getLogs() const;

    void setProcessId(int id) { process_id_ = id; }
    int getProcessId() const { return process_id_; }

    using UpdateCallback = std::function<void(const std::string& name, int coreId, const std::string& progress)>;

    void setUpdateCallback(UpdateCallback cb);

private:
    std::string name;
    int process_id_ = -1;
    int totalPrints;
    int printsCompleted = 0;
    int assignedCore = -1;
    // std::ofstream logFile;

    std::vector<std::string> logs;

    std::string getCurrentTimestamp();

    UpdateCallback updateCallback;
};