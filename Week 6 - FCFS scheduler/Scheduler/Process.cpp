#include "Process.h"
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <iostream>

Process::Process(const std::string& name, int totalPrints)
    : name(name), totalPrints(totalPrints) {

    // directory creation
    std::string logPath = "logs/" + name + "_logs.txt";  
    std::cout << "Creating log file at: " << std::filesystem::absolute(name + "_logs.txt") << "\n";
    std::filesystem::create_directory("logs");
    logFile.open(logPath);
}

void Process::executePrint(int coreId) {
    assignedCore = coreId;
    logFile << "[ ] (" << getCurrentTimestamp() << ") Core:" << coreId
        << " \"Hello world from " << name << "!\"\n";
    printsCompleted++;
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