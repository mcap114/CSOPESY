#include "Screen.h"
#include <ctime>

Screen::Screen(const std::string& name, bool is_running)
    : name_(name), is_running_(is_running),
    creation_time_(std::chrono::system_clock::now()) {
}

std::string Screen::getCreationTime() const {
    auto time = std::chrono::system_clock::to_time_t(creation_time_);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}

std::string Screen::getProgress() const {
    return std::to_string(current_line_) + "/" + std::to_string(total_lines_);
}

std::string Screen::render() const {
    std::ostringstream oss;
    oss << "╭────────────────────────────────────────────╮\n"
        << "│               \033[38;5;87m♦ DASHBOARD ♦\033[0m                │\n"
        << "├────────────────────────────────────────────┤\n"
        << "│SCREEN: " << std::left << std::setw(16) << name_ << "│\n"
        << "├────────────────────────────────────────────┤\n"
        << "│ Status:   " << std::setw(16) << (is_running_ ? "RUNNING" : "STOPPED") << "│\n"
        << "│ Progress: " << std::setw(16) << getProgress() << "│\n"
        << "│ Created:  " << std::setw(16) << getCreationTime() << "│\n"
        << "╰────────────────────────────────────────────╯\n\n";
    return oss.str();
}