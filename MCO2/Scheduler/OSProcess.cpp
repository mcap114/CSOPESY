#include "OSProcess.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

OsProcess::OsProcess(const std::string& name, int totalPrints) 
    : name(name), totalPrints(totalPrints) {
}

void OsProcess::setUpdateCallback(UpdateCallback cb) {
    updateCallback = std::move(cb);
}

void OsProcess::executePrint(int coreId) {
    assignedCore = coreId;

    if (sleep_ticks_remaining > 0) {
        sleep_ticks_remaining--;

        std::string sleep_msg = "[" + getCurrentTimestamp() + "] Core:" +
            std::to_string(coreId) + " " + name + " is sleeping... (" +
            std::to_string(sleep_ticks_remaining) + " ticks remaining)";
        logs.push_back(sleep_msg);

        return;
    }

    simulateInstruction();
    std::string msg = "[" + getCurrentTimestamp() + "] Core:" +
        std::to_string(coreId) + " \"Hello world from " + name + "!\"";
    logs.push_back(msg);
    printsCompleted++;

    if (updateCallback) {
        std::string progress = std::to_string(printsCompleted) + "/" + std::to_string(totalPrints);
        updateCallback(name, coreId, progress);
    }
}

const std::vector<std::string>& OsProcess::getLogs() const {
    return logs;
}

bool OsProcess::isCompleted() const {
    return printsCompleted >= totalPrints;
}

const std::string& OsProcess::getName() const {
    return name;
}

int OsProcess::getCoreId() const {
    return assignedCore;
}

void OsProcess::simulateInstruction() {
    instruction_line++;
}

int OsProcess::getTotalInstructions() const {
    return totalPrints;
}

std::string OsProcess::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &in_time_t);
#else
    localtime_r(&in_time_t, &tm_buf);
#endif

    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
}

void OsProcess::generateRandomInstructions(int count) {
    static const std::vector<std::string> varNames = { "x", "y", "z", "a", "b", "c", "sum", "res" };

    for (int i = 0; i < count; ++i) {
        Instruction instr;
        int type = rand() % 4;

        switch (type) {
        case 0:
            instr.type = InstructionType::DECLARE;
            instr.target = varNames[rand() % varNames.size()];
            instr.operand1 = std::to_string(rand() % 100);
            break;

        case 1:
            instr.type = InstructionType::ADD;
            instr.target = varNames[rand() % varNames.size()];
            instr.operand1 = varNames[rand() % varNames.size()];
            instr.operand2 = std::to_string(rand() % 50);
            break;

        case 2:
            instr.type = InstructionType::SUBTRACT;
            instr.target = varNames[rand() % varNames.size()];
            instr.operand1 = varNames[rand() % varNames.size()];
            instr.operand2 = std::to_string(rand() % 50);
            break;

        case 3:
            instr.type = InstructionType::PRINT;
            instr.message = "Hello world from " + name + "!";
            break;
        }

        instructions.push_back(instr);
    }
}

void OsProcess::executeNextInstruction(int coreId) {
    if (instruction_ptr >= instructions.size()) return;

    const Instruction& instr = instructions[instruction_ptr];
    std::string log;

    auto getValue = [this](const std::string& operand) -> uint16_t {
        if (variables.count(operand)) {
            return variables[operand];
        }
        try {
            return static_cast<uint16_t>(std::stoi(operand));
        }
        catch (...) {
            return 0;
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

void OsProcess::simulateMemoryViolation(uintptr_t invalidAddr) {
    memoryViolation = true;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm_buf;
    #ifdef _WIN32
        localtime_s(&tm_buf, &now);
    #else
        localtime_r(&now, &tm_buf);
    #endif
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm_buf);

    violationTimestamp = buffer;
    std::stringstream ss;
    ss << "0x" << std::hex << invalidAddr;
    invalidAddress = ss.str();
}

bool OsProcess::hasMemoryViolation() const {
    return memoryViolation;
}

std::string OsProcess::getViolationTimestamp() const {
    return violationTimestamp;
}

std::string OsProcess::getInvalidAddress() const {
    return invalidAddress;
}