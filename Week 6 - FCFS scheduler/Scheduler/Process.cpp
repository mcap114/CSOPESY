#include "Process.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

Process::Process(const std::string& name, int totalPrints)
    : name(name), totalPrints(totalPrints) {

}

void Process::executePrint(int coreId) {
    assignedCore = coreId;
    std::string msg = "[" + getCurrentTimestamp() + "] Core:" +
                      std::to_string(coreId) + " \"Hello world from " + name + "!\"";
    logs.push_back(msg);  
    printsCompleted++;
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