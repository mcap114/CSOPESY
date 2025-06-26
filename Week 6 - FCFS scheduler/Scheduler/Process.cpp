#include "Process.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

Process::Process(const std::string& name, int totalPrints)
    : name(name), totalPrints(totalPrints) {
}

void Process::setUpdateCallback(UpdateCallback cb) {
    updateCallback = std::move(cb);
}

void Process::setSleepTicks(uint8_t ticks) {
    sleep_ticks_remaining = ticks;
}

void Process::executePrint(int coreId) {
    assignedCore = coreId;

    // ✅ Non-blocking sleep: just decrement tick and yield
    if (sleep_ticks_remaining > 0) {
        sleep_ticks_remaining--;

        std::string sleep_msg = "[" + getCurrentTimestamp() + "] Core:" +
            std::to_string(coreId) + " " + name + " is sleeping... (" +
            std::to_string(sleep_ticks_remaining) + " ticks remaining)";
        logs.push_back(sleep_msg);

        return; // ✅ Yield CPU
    }

    simulateInstruction();

    std::string msg = "[" + getCurrentTimestamp() + "] Core:" +
                      std::to_string(coreId) + " \"Hello world from " + name + "!\"";
    logs.push_back(msg);
    printsCompleted++;

    if (updateCallback) {
    std::string progress = std::to_string(printsCompleted) + "/" + std::to_string(totalPrints);
    updateCallback(name, coreId, progress, printsCompleted);  // pass instruction line
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

void Process::simulateInstruction() {
    instruction_line++;
    // Optional: add more logic here later
}

int Process::getInstructionLine() const {
    return instruction_line;
}

int Process::getTotalInstructions() const {
    return totalPrints;
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
