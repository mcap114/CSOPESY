#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <vector>
#include <functional>
#include <cstdint>

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

    int getInstructionLine() const;
    int getTotalInstructions() const;

    using UpdateCallback = std::function<void(const std::string& name, int coreId, const std::string& progress, int instructionLine)>;
    void setUpdateCallback(UpdateCallback cb);

    void setSleepTicks(uint8_t ticks); // ✅ Added

private:
    std::string name;
    int process_id_ = -1;
    int totalPrints;
    int printsCompleted = 0;
    int assignedCore = -1;

    std::vector<std::string> logs;

    int instruction_line = 0;
    uint8_t sleep_ticks_remaining = 0; // ✅ Added for sleep support

    void simulateInstruction();
    std::string getCurrentTimestamp();

    UpdateCallback updateCallback;
};
