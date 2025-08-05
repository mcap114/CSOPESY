#include "OSProcess.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const uint32_t MEMORY_LIMIT = 1 << 16;  // 64KB limit (or set based on your system design)

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
    return instruction_ptr >= static_cast<int>(instructions.size());
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
        if (variables.size() >= MAX_VARIABLES) {
            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " DECLARE ignored: symbol table full";
            break;
        }

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
    case InstructionType::WRITE: {
        try {
            uint32_t address = std::stoul(instr.target, nullptr, 16);  // e.g., 0x1000
            uint16_t value = getValue(instr.operand1);

            if (address + 1 >= MEMORY_LIMIT) {
                simulateMemoryViolation(address);
                return;
            }

            value = std::clamp(value, static_cast<uint16_t>(0), static_cast<uint16_t>(65535));
            simulatedMemory[address] = value;

            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " WRITE(" + instr.target + ", " + instr.operand1 + ")";
        } catch (...) {
            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " WRITE failed due to invalid address/value";
        }
        break;
    }
    case InstructionType::READ: {
        try {
            if (variables.size() >= 32) {
                log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                    + " READ ignored: symbol table full";
                break;
            }

            uint32_t address = std::stoul(instr.operand1, nullptr, 16);

            if (address + 1 >= MEMORY_LIMIT) {
                simulateMemoryViolation(address);
                return;
            }

            uint16_t value = simulatedMemory.count(address) ? simulatedMemory[address] : 0;
            if (variables.size() >= MAX_VARIABLES && variables.find(instr.target) == variables.end()) {
                log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                    + " READ ignored: symbol table full";
                break;
            }

            variables[instr.target] = value;

            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " READ(" + instr.target + ", " + instr.operand1 + ") = " + std::to_string(value);
        } catch (...) {
            log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
                + " READ failed due to invalid address";
        }
        break;
    }
    case InstructionType::PRINT: {
        std::string finalMsg = instr.message;

        for (const auto& [var, val] : variables) {
            std::string placeholder = var;
            size_t pos;
            while ((pos = finalMsg.find(placeholder)) != std::string::npos) {
                finalMsg.replace(pos, placeholder.length(), std::to_string(val));
            }
        }

        log = "[" + getCurrentTimestamp() + "] Core:" + std::to_string(coreId)
            + " PRINT: " + finalMsg;
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

void OsProcess::simulateMemoryViolation(uint32_t address) {
    simulateMemoryViolation(static_cast<uintptr_t>(address));
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

void OsProcess::parseUserInstructions(const std::vector<std::string>& lines) {
    for (const std::string& line : lines) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        Instruction instr;

        if (keyword == "DECLARE") {
            instr.type = InstructionType::DECLARE;
            iss >> instr.target >> instr.operand1;
        }
        else if (keyword == "ADD") {
            instr.type = InstructionType::ADD;
            iss >> instr.target >> instr.operand1 >> instr.operand2;
        }
        else if (keyword == "SUBTRACT") {
            instr.type = InstructionType::SUBTRACT;
            iss >> instr.target >> instr.operand1 >> instr.operand2;
        }
        else if (keyword == "WRITE") {
            instr.type = InstructionType::WRITE;
            iss >> instr.target >> instr.operand1;
        }
        else if (keyword == "READ") {
            instr.type = InstructionType::READ;
            iss >> instr.target >> instr.operand1;
        }
        else if (keyword == "PRINT") {
            instr.type = InstructionType::PRINT;
            
            // Find the opening parenthesis
            size_t openParen = line.find('(');
            if (openParen == std::string::npos) {
                continue; // Invalid format, skip
            }
            
            // Find the closing parenthesis
            size_t closeParen = line.rfind(')');
            if (closeParen == std::string::npos || closeParen <= openParen) {
                continue; // Invalid format, skip
            }
            
            // Extract the content between parentheses
            std::string content = line.substr(openParen + 1, closeParen - openParen - 1);
            
            // Remove any quotes if present
            size_t firstQuote = content.find('"');
            size_t lastQuote = content.rfind('"');
            
            if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote) {
                // Extract text between quotes
                instr.message = content.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                
                // Check for concatenation with +
                size_t plusPos = content.find('+', lastQuote);
                if (plusPos != std::string::npos) {
                    std::string varPart = content.substr(plusPos + 1);
                    // Trim whitespace from variable name
                    varPart.erase(0, varPart.find_first_not_of(" \t"));
                    varPart.erase(varPart.find_last_not_of(" \t") + 1);
                    instr.message += " " + varPart; // Add space and variable name
                }
            } else {
                // No quotes, use the entire content
                instr.message = content;
            }
        }
        else {
            continue; // Unknown command, ignore
        }

        instructions.push_back(instr);
    }
}