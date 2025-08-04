#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "ScreenManager.h"
#include "FCFSScheduler.h"
#include "RRScheduler.h"
#include "BaseScheduler.h"
#include "../Core/Config.h"
#include "../Core/CPUTimer.h"             
#include "../Utilities/ReportGenerator.h"

class CLI {
public:
    CLI();
	CLI(const Config& config);
    ~CLI();
    void run();

    enum class AppState { MAIN_MENU, IN_SCREEN };

private:
    bool is_initialized_ = false;
	Config config_; 

    // State tracking
    int next_process_id_ = 1;
    AppState current_state_;
    std::string active_screen_name_;
    ScreenManager screen_manager_;
    std::unique_ptr<BaseScheduler> scheduler_;
    CPUTimer cpu_timer_;
    std::unique_ptr<ReportGenerator> report_gen_;
    uint64_t pages_paged_in_ = 0;
    uint64_t pages_paged_out_ = 0;

    // Command map with argument support
    std::unordered_map<std::string, std::function<void(const std::string&)>> commands;

    // Core methods
    void printHeader(bool show_prompt = true);
    void clearScreen();
    void returnToMainMenu();

    // Command handlers (all with args)
    void handleInitialize(const std::string& args);
    void handleScreen(const std::string& args);
    void handleSchedulerTest(const std::string& args);
    void handleSchedulerStop(const std::string& args);
    void handleReportUtil(const std::string& args);
    void handleProcessSMI(const std::string& args);
    void handleVmstat(const std::string& args);
    void handleExit(const std::string& args);
    void handleSleep(const std::string& args);
    void handleFor(const std::string& args);
    // Helper functions
    void createProcessScreen(const std::string& processName, int totalPrints);
    bool parseScreenArgs(const std::string& args, char& mode, std::string& name);
};