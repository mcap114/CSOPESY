#include "Screen.h"
#include <ctime>
#include <algorithm>

Screen::Screen(const std::string& name, bool is_running)
    : name_(name), is_running_(is_running),
    creation_time_(std::chrono::system_clock::now()) {
}

std::string Screen::getCreationTime() const {
    auto time = std::chrono::system_clock::to_time_t(creation_time_);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%m/%d/%Y %I:%M:%S %p");
    return oss.str();
}

void Screen::addProcess(const ProcessInfo& process) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    processes_.push_back(process);
}

void Screen::updateProcess(const std::string& process_name, const std::string& status,
    int core, const std::string& progress, int instructionLine) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    auto it = std::find_if(processes_.begin(), processes_.end(),
        [&](const ProcessInfo& p) { return p.name == process_name; });

    if (it != processes_.end()) {
        it->status = status;
        it->core = core;
        it->progress = progress;
        it->instruction_line = instructionLine;
    }
}

void Screen::removeProcess(const std::string& process_name) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    processes_.erase(std::remove_if(processes_.begin(), processes_.end(),
        [&](const ProcessInfo& p) { return p.name == process_name; }),
        processes_.end());
}

std::string Screen::render() const {
    std::ostringstream oss;
    std::lock_guard<std::mutex> lock(process_mutex_);

    if (processes_.empty()) {
        oss << "No process info available.\n";
        return oss.str();
    }

    const ProcessInfo& proc = processes_.front();  

    int id = 0;
    std::string name = proc.name;
    size_t underscore = name.find_last_of('_');
    if (underscore != std::string::npos) {
        try {
            id = std::stoi(name.substr(underscore + 1));
        } catch (...) {
            id = 0;
        }
    }

    oss << "\nProcess: " << proc.name << "\n";
    oss << "ID: " << proc.id << "\n\n"; 
    oss << "Current instruction line: " << proc.instruction_line << "\n";
    oss << "Lines of code: " << proc.total_instructions << "\n";
    oss << "root:\\> ";

    return oss.str();
}

std::string Screen::renderProcessList() const {
    std::lock_guard<std::mutex> lock(process_mutex_);
    std::ostringstream oss;

    if (processes_.empty()) {
        oss << "│ No processes running                                │\n";
        return oss.str();
    }

    oss << "│ RUNNING PROCESSES:                                  │\n";
    for (const auto& process : processes_) {
        oss << "│ • " << std::left << std::setw(16) << process.name
            << " Core:" << std::setw(2) << process.core
            << " " << std::setw(10) << (is_running_ ? "RUNNING" : "COMPLETE")
            << " " << process.progress << std::setw(5) << " │\n";
    }

    return oss.str();
}