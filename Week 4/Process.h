#pragma once
#include <string>

class Process {
public:
    Process(const std::string& name, int total_instructions)
        : name_(name),
        total_instructions_(total_instructions) {
    }

    std::string getName() {

    }

    // Execution control
    void executeNextInstruction() {
        if (current_instruction_ < total_instructions_)
            current_instruction_++;
    }

    // Progress reporting
    int getCurrentInstruction() const { return current_instruction_; }
    int getTotalInstructions() const { return total_instructions_; }

    // Console output 
    std::string getExecutionStatus() const {
        return "Executing line: " + std::to_string(current_instruction_) +
            "/" + std::to_string(total_instructions_);
    }

private:
    std::string name_;              // Process identifier
    int current_instruction_ = 0;   // Tracks current line (0-indexed)
    int total_instructions_;        // Total lines in program
};

