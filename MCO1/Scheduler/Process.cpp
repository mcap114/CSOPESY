#include "Process.h"
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <iostream>
#include "../Core/CPUTimer.h"

Process::Process(const std::string& name, int totalPrints)
    : name(name), totalPrints(totalPrints) {

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
    localtime_r(&in_time_t, &tm_buf);
#endif

    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
}

// =================== SLEEP & FOR COMMANDS ===================

void Process::handleSleep(unsigned long ticks) {
    sleepStartTick = CPUTimer::getTicks();
    sleepDuration = ticks;
    sleeping = true;

    logFile << "[SLEEPING] (" << getCurrentTimestamp() << ") Will sleep for " << ticks << " ticks.\n";
}

void Process::handleForLoop(const std::vector<std::string>& body, int repeat) {
    loopBody = body;
    forLoopLimit = repeat;
    forLoopCounter = 0;
    inForLoop = true;

    logFile << "[FOR] (" << getCurrentTimestamp() << ") Starting for-loop for " << repeat << " iterations.\n";
}

void Process::step() {
    if (isCompleted()) return;

    if (sleeping) {
        unsigned long now = CPUTimer::getTicks();
        if (now - sleepStartTick >= sleepDuration) {
            sleeping = false;
            logFile << "[SLEEP DONE] (" << getCurrentTimestamp() << ") Woke up after " << sleepDuration << " ticks.\n";
        } else {
            logFile << "[SLEEPING] (" << getCurrentTimestamp() << ") Still sleeping...\n";
            return;
        }
    }

    if (inForLoop) {
        if (forLoopCounter < forLoopLimit) {
            for (const auto& cmd : loopBody) {
                logFile << "[FOR] Iteration " << (forLoopCounter + 1) << ": " << cmd << "\n";
            }
            forLoopCounter++;
        } else {
            logFile << "[FOR DONE] (" << getCurrentTimestamp() << ") Finished all " << forLoopLimit << " iterations.\n";
            inForLoop = false;
        }
        return;
    }

    executePrint(getCoreId());
}
