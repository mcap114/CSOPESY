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


enum class InstructionType {
    DECLARE,
    ADD,
    SUBTRACT,
    PRINT
};

struct Instruction {
    InstructionType type;
    std::string target;     // var1
    std::string operand1;   // var2 or literal
    std::string operand2;   // var3 or literal (only for ADD/SUBTRACT)
    std::string message;    // used for PRINT
};

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

    void generateRandomInstructions(int count);  // to populate instructions
    void executeNextInstruction(int coreId);     // for running 1 instruction per tick

    int getInstructionPointer() const { return instruction_ptr; }
    int getInstructionCount() const { return instructions.size(); }
    int getInstructionLine() const { return instruction_ptr; }



private:
    std::string name;
    int process_id_ = -1;
    int totalPrints;
    int printsCompleted = 0;
    int assignedCore = -1;
    // std::ofstream logFile;

    std::vector<std::string> logs;

    std::string getCurrentTimestamp();

    std::vector<Instruction> instructions;
    std::unordered_map<std::string, uint16_t> variables;

    int instruction_ptr = 0;  // current instruction line


    UpdateCallback updateCallback;
};