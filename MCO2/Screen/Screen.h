#pragma once
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdint>

class Screen {
public:
    Screen(const std::string& name, bool is_running);

    //core
	void tick(); // simulate one CPU cycle
    bool isFinished() const;
	bool isSleeping() const;

    //getters
    std::string getName() const { return name_; }
	bool isRunning() const { return is_running_; }
    std::string getCreationTime() const;
    std::string getProgress() const;
    std::string render() const;

    //state management
    void setRunning(bool running) { is_running_ = running; }
    // void updateProgress(int current, int total = 10);
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

    // Output buffer (from PRINT)
    std::vector<std::string> output_log_;

    // Helpers
	void executeInstruction(const Instruction& instr);
    uint16_t resolveValue(const std::string& token);
    void sleepFor(uint8_t ticks);
    void initInstructions();
};