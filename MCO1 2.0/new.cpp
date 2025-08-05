#pragma once
#include <string>
#include <chrono>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdint>

class Screen {
public:
    Screen(const std::string& name, bool is_running);

    // Core
    void tick(); // simulate one CPU cycle
    bool isFinished() const;
    bool isSleeping() const;

    // Getters
    std::string getName() const { return name_; }
    bool isRunning() const { return is_running_; }
    std::string getCreationTime() const;
    std::string getProgress() const;
    std::string render() const;

    // State mgmt
    void setRunning(bool running) { is_running_ = running; }
    void updateProgress(int current, int total = 10);
    void markAsStopped();

private:
    std::string name_;
    bool is_running_;
    std::chrono::system_clock::time_point creation_time_;

    // Execution state
    int instruction_pointer_ = 0;
    int sleep_ticks_remaining_ = 0;

    // Process instructions
    struct Instruction {
        std::string type;
        std::vector<std::string> args;
    };
    std::vector<Instruction> instructions_;

    // FOR loop support
    struct LoopFrame {
        int startIndex;
        int remainingRepeats;
    };
    std::vector<LoopFrame> loop_stack_;

    // Variable memory
    std::unordered_map<std::string, uint16_t> variables_;

    // Output buffer (e.g., from PRINT)
    std::vector<std::string> output_log_;

    // Helpers
    void executeInstruction(const Instruction& instr);
    uint16_t resolveValue(const std::string& token);
    void sleepFor(uint8_t ticks);
    void initInstructions(); // populates with randomized instruction list
};

#include "Screen.h"
#include <ctime>
#include <iomanip>
#include <iostream>

Screen::Screen(const std::string& name, bool is_running)
    : name_(name), is_running_(is_running), creation_time_(std::chrono::system_clock::now()) {
    initInstructions();
}

void Screen::initInstructions() {
    // For now, simple hardcoded program. You’ll randomize this in scheduler-start.
    instructions_ = {
        {"DECLARE", {"x", "10"}},
        {"ADD", {"x", "x", "5"}},
        {"SLEEP", {"2"}},
        {"SUBTRACT", {"x", "x", "1"}},
        {"PRINT", {"\"Hello world from " + name_ + "!\""}}
    };
}

void Screen::tick() {
    if (!is_running_ || isFinished()) return;

    if (sleep_ticks_remaining_ > 0) {
        sleep_ticks_remaining_--;
        return;
    }

    if (instruction_pointer_ >= instructions_.size()) {
        is_running_ = false;
        return;
    }

    const auto& instr = instructions_[instruction_pointer_];
    executeInstruction(instr);
    instruction_pointer_++;
}

void Screen::executeInstruction(const Instruction& instr) {
    if (instr.type == "DECLARE") {
        variables_[instr.args[0]] = static_cast<uint16_t>(std::stoi(instr.args[1]));
    }
    else if (instr.type == "ADD" || instr.type == "SUBTRACT") {
        auto a = resolveValue(instr.args[1]);
        auto b = resolveValue(instr.args[2]);
        int result = (instr.type == "ADD") ? (a + b) : (a - b);
        if (result < 0) result = 0;
        if (result > UINT16_MAX) result = UINT16_MAX;
        variables_[instr.args[0]] = static_cast<uint16_t>(result);
    }
    else if (instr.type == "SLEEP") {
        sleepFor(std::stoi(instr.args[0]));
    }
    else if (instr.type == "PRINT") {
        std::string msg = instr.args[0];
        // Replace +var
        size_t plus = msg.find('+');
        if (plus != std::string::npos) {
            std::string base = msg.substr(1, plus - 2); // remove quotes
            std::string var = msg.substr(plus + 1, msg.length() - plus - 2);
            msg = base + std::to_string(variables_[var]);
        }
        output_log_.push_back(msg);
    }
    // FOR support comes next if needed
}

uint16_t Screen::resolveValue(const std::string& token) {
    if (isdigit(token[0])) return static_cast<uint16_t>(std::stoi(token));
    return variables_[token];
}

void Screen::sleepFor(uint8_t ticks) {
    sleep_ticks_remaining_ = ticks;
}

bool Screen::isFinished() const {
    return instruction_pointer_ >= instructions_.size();
}

bool Screen::isSleeping() const {
    return sleep_ticks_remaining_ > 0;
}

void Screen::markAsStopped() {
    is_running_ = false;
}

std::string Screen::getCreationTime() const {
    auto time = std::chrono::system_clock::to_time_t(creation_time_);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}

void Screen::updateProgress(int current, int total) {
    // Optional for your own rendering
}

std::string Screen::getProgress() const {
    return std::to_string(instruction_pointer_) + "/" + std::to_string(instructions_.size());
}

std::string Screen::render() const {
    std::ostringstream oss;
    oss << "??????????????????????????????????????????????\n"
        << "?               \033[38;5;87m? DASHBOARD ?\033[0m                ?\n"
        << "??????????????????????????????????????????????\n"
        << "?SCREEN: " << std::left << std::setw(16) << name_ << "?\n"
        << "??????????????????????????????????????????????\n"
        << "? Status:   " << std::setw(16) << (is_running_ ? "RUNNING" : "STOPPED") << "?\n"
        << "? Progress: " << std::setw(16) << getProgress() << "?\n"
        << "? Created:  " << std::setw(16) << getCreationTime() << "?\n"
        << "??????????????????????????????????????????????\n\n";

    for (const auto& line : output_log_) {
        oss << line << "\n";
    }

    return oss.str();
}
