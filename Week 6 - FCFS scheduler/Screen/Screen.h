#pragma once
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

class Screen {
public:
    Screen(const std::string& name, bool is_running);

    // Getters
    std::string getName() const { return name_; }
    bool isRunning() const { return is_running_; }
    std::string getCreationTime() const;
    std::string getProgress() const;

    // State management
    void setRunning(bool running) { is_running_ = running; }
    void updateProgress(int current, int total = 10) {
        current_line_ = current;
        total_lines_ = total;
    }

    // Rendering
    std::string render() const;

private:
    std::string name_;
    bool is_running_;
    int current_line_ = 0;
    int total_lines_ = 10;
    std::chrono::system_clock::time_point creation_time_;
};