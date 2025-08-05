#pragma once
#include <atomic>
#include <thread>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "..\Screen\ScreenManager.h"
#include "..\Config\Config.h"
#include "../Scheduler/Scheduler.h"
#include "../Scheduler/SchedulerFactory.h"

class CLI {
public:
	CLI();
	CLI(const Config& config); // constructor to initialize config_
	//~CLI();
	void run();

	enum class OsState { MAIN_MENU, IN_SCREEN, EXIT };

private:
	bool is_initialized_ = false;

	std::atomic<bool> ticking_ = false;
	std::atomic<bool> generating_processes_ = false;
	std::atomic<uint64_t> tick_count_ = 0;

	std::thread tick_thread_;
	std::thread generator_thread_;
	int process_counter_ = 1;

	std::unique_ptr<Scheduler> scheduler_;

	OsState current_state_;
	std::string active_screen_name_;
	ScreenManager screen_manager_;
	Config config_; 

	std::unordered_map<std::string, std::function<void(const std::string&)>> commands;

	void printHeader(bool show_prompt = true);
	void clearScreen();
	void returnToMainMenu();

	void handleInitialize(const std::string& args);
	void handleScreen(const std::string& args);
	void handleSchedulerTest(const std::string& args);
	void handleSchedulerStart(const std::string& args);
	void handleSchedulerStop(const std::string& args);
	void handleReportUtil(const std::string& args);
	void handleExit(const std::string& args);

	bool parseScreenArgs(const std::string& args, char& mode, std::string& name);
};