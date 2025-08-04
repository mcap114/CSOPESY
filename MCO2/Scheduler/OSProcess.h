#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <vector>
#include <functional>
#include <unordered_map>
#include <cstdint>

enum class InstructionType {
    DECLARE,
    ADD,
    SUBTRACT,
    PRINT
};

struct Instruction {
    InstructionType type;
    std::string target;
    std::string operand1;
    std::string operand2;
    std::string message;
};

class OsProcess { // ✅ Renamed class
public:
    OsProcess(const std::string& name, int totalPrints);
    void executePrint(int coreId);
    bool isCompleted() const;
    const std::string& getName() const;
    int getCoreId() const;
    const std::vector<std::string>& getLogs() const;

    void setProcessId(int id) { process_id_ = id; }
    int getProcessId() const { return process_id_; }

    int getTotalInstructions() const;

    using UpdateCallback = std::function<void(const std::string& name, int coreId, const std::string& progress)>;

    void setUpdateCallback(UpdateCallback cb);

    void generateRandomInstructions(int count);
    void executeNextInstruction(int coreId);

    int getInstructionPointer() const { return instruction_ptr; }
    int getInstructionCount() const { return static_cast<int>(instructions.size()); }
    int getInstructionLine() const { return instruction_ptr; }
    void setSleepTicks(uint8_t ticks); //Added

private:
    std::string name;
    int process_id_ = -1;
    int totalPrints;
    int printsCompleted = 0;
    int assignedCore = -1;

    std::vector<std::string> logs;

    std::string getCurrentTimestamp();

    int instruction_line = 0;
    uint8_t sleep_ticks_remaining = 0;
    void simulateInstruction();

    std::vector<Instruction> instructions;
    std::unordered_map<std::string, uint16_t> variables;

    int instruction_ptr = 0;

    UpdateCallback updateCallback;
};
