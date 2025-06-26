#include "Process.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include <cstdlib> // for rand()
#include <ctime>   // for time()


Process::Process(const std::string& name, int totalPrints)
    : name(name), totalPrints(totalPrints) {

}

void Process::setUpdateCallback(UpdateCallback cb) {
    updateCallback = std::move(cb);
}


void Process::executePrint(int coreId) {
    assignedCore = coreId;
    std::string msg = "[" + getCurrentTimestamp() + "] Core:" +
                      std::to_string(coreId) + " \"Hello world from " + name + "!\"";
    logs.push_back(msg);  
    printsCompleted++;

    if (updateCallback) {
        std::string progress = std::to_string(printsCompleted) + "/" + std::to_string(totalPrints);
        updateCallback(name, coreId, progress);
    }
}

const std::vector<std::string>& Process::getLogs() const {
    return logs;
}

bool Process::isCompleted() const {
    return printsCompleted >= totalPrints;
}

const std::string& Process::getName() const {
    return name;
}

int Process::getCoreId() const {
    return assignedCore;
}

std::string Process::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &in_time_t);
#else
    localtime_r(&in_time_t, &tm_buf);  // POSIX version
#endif

    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
}

void Process::generateRandomInstructions(int count) {
    static const std::vector<std::string> varNames = {"x", "y", "z", "a", "b", "c", "sum", "res"};

    for (int i = 0; i < count; ++i) {
        Instruction instr;
        int type = rand() % 4; // 0–3 for 4 types

        switch (type) {
            case 0: // DECLARE
                instr.type = InstructionType::DECLARE;
                instr.target = varNames[rand() % varNames.size()];
                instr.operand1 = std::to_string(rand() % 100); // e.g., DECLARE(x, 5)
                break;

            case 1: // ADD
                instr.type = InstructionType::ADD;
                instr.target = varNames[rand() % varNames.size()];
                instr.operand1 = varNames[rand() % varNames.size()];
                instr.operand2 = std::to_string(rand() % 50); // can be literal
                break;

            case 2: // SUBTRACT
                instr.type = InstructionType::SUBTRACT;
                instr.target = varNames[rand() % varNames.size()];
                instr.operand1 = varNames[rand() % varNames.size()];
                instr.operand2 = std::to_string(rand() % 50); // can be literal
                break;

            case 3: // PRINT
                instr.type = InstructionType::PRINT;
                instr.message = "Hello world from " + name + "!";
                break;
        }

        instructions.push_back(instr);
    }
}

void Process::executeNextInstruction(int coreId) {
    if (instruction_ptr >= instructions.size()) return; // already done

    const Instruction& instr = instructions[instruction_ptr];
    std::string log;

    // Helper lambda to safely get the value of an operand (variable or number)
    auto getValue = [this](const std::string& operand) -> uint16_t {
        if (variables.count(operand)) {
            return variables[operand];
        }
        try {
            return static_cast<uint16_t>(std::stoi(operand));
        } catch (...) {
            return 0; // fallback if not a number and not a declared variable
        }
    };

    switch (instr.type) {
        case InstructionType::DECLARE: {
            uint16_t value = getValue(instr.operand1);
            variables[instr.target] = value;
            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " DECLARE(" + instr.target + ", " + instr.operand1 + ")";
            break;
        }
        case InstructionType::ADD: {
            uint16_t op1 = getValue(instr.operand1);
            uint16_t op2 = getValue(instr.operand2);
            variables[instr.target] = op1 + op2;
            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " ADD(" + instr.target + ", " + instr.operand1 + ", " + instr.operand2 + ")";
            break;
        }
        case InstructionType::SUBTRACT: {
            uint16_t op1 = getValue(instr.operand1);
            uint16_t op2 = getValue(instr.operand2);
            uint16_t result = (op1 > op2) ? (op1 - op2) : 0;
            variables[instr.target] = result;
            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " SUBTRACT(" + instr.target + ", " + instr.operand1 + ", " + instr.operand2 + ")";
            break;
        }
        case InstructionType::PRINT: {
            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " PRINT: " + instr.message;
            break;
        }
    }

    logs.push_back(log);
    printsCompleted++;
    instruction_ptr++;

    if (updateCallback) {
        std::string progress = std::to_string(instruction_ptr) + "/" + std::to_string(instructions.size());
        updateCallback(name, coreId, progress);
    }
}

