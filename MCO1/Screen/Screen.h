#pragma once
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <mutex>

struct ProcessInfo {
    int id = 0;
    std::string name;
    std::string status;
    int core;
    std::string progress;
    std::string creation_time;
    int instruction_line = 0;
    int total_instructions = 50;
};

class Screen {
public:
    Screen(const std::string& name, bool is_running);

    // Getters
    std::string getName() const { return name_; }
    bool isRunning() const { return is_running_; }
    std::string getCreationTime() const;
    const std::vector<ProcessInfo>& getProcesses() const {
        return processes_;
    }

    // State management
    void setRunning(bool running) { is_running_ = running; }  

    // Process management
    void addProcess(const ProcessInfo& process);
    void updateProcess(const std::string& process_name, const std::string& status,
                   int core, const std::string& progress, int instructionLine);
    void removeProcess(const std::string& process_name);
   

    // Rendering
    std::string render() const;
    std::string renderProcessList() const;
    std::vector<ProcessInfo>& getProcessesRef() { return processes_; }


private:
    std::string name_;
    bool is_running_;
    std::chrono::system_clock::time_point creation_time_;
    std::vector<ProcessInfo> processes_;
    mutable std::mutex process_mutex_;
};