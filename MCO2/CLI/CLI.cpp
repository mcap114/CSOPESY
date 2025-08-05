#include "../CLI/CLI.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

CLI::CLI() : current_state_(OsState::MAIN_MENU) {

    commands["initialize"] = [this](const std::string& args) { handleInitialize(args); };
    commands["screen"] = [this](const std::string& args) { handleScreen(args); };
    commands["scheduler-test"] = [this](const std::string& args) { handleSchedulerTest(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["clear"] = [this](const std::string& args) { clearScreen(); };
    commands["exit"] = [this](const std::string& args) { handleExit(args); };
}

CLI::CLI(const Config& config)
    : config_(config), current_state_(OsState::MAIN_MENU) {
    commands["initialize"] = [this](const std::string& args) { handleInitialize(args); };
    commands["screen"] = [this](const std::string& args) { handleScreen(args); };
    commands["scheduler-start"] = [this](const std::string& args) { handleSchedulerStart(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["clear"] = [this](const std::string& args) { clearScreen(); };
    commands["exit"] = [this](const std::string& args) { handleExit(args); };
}

void CLI::run() {
    int cpuCycles = 0;
    printHeader();
    std::string input;

    while (current_state_ != OsState::EXIT) {
		cpuCycles++;
        std::cout << cpuCycles;

        if (current_state_ == OsState::MAIN_MENU) {
            std::cout << "\nEnter a command: ";
        }
        std::getline(std::cin, input);

        // Handle exit command differently based on state
        if (current_state_ == OsState::IN_SCREEN && input == "exit") {
            returnToMainMenu();
            continue;
        }

        // Split into command and arguments
        auto space_pos = input.find(' ');
        std::string cmd = input.substr(0, space_pos);
        std::string args = (space_pos != std::string::npos) ? input.substr(space_pos + 1) : "";

        if (commands.find(cmd) != commands.end()) {
            commands[cmd](args);
        }
        else {
            std::cout << "Unrecognized command. Try again.\n";
        }
	}
}

void CLI::printHeader(bool show_prompt) {
    std::cout << R"(
.------..------..------..------..------..------..------.
|C.--. ||S.--. ||O.--. ||P.--. ||E.--. ||S.--. ||Y.--. |
| :/\: || :/\: || :/\: || :/\: || (\/) || :/\: || (\/) |
| :\/: || :\/: || :\/: || (__) || :\/: || :\/: || :\/: |
| '--'C|| '--'S|| '--'O|| '--'P|| '--'E|| '--'S|| '--'Y|
`------'`------'`------'`------'`------'`------'`------'
)" << std::endl;

    std::cout << "Hello, Welcome to CSOPESY commandline!\n";
    std::cout << "Type 'exit' to quit, 'clear' to clear the screen\n";

}

void CLI::clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
    printHeader(false);  // Don't show prompt after clear
}

void CLI::handleInitialize(const std::string& args) {
    if (is_initialized_) {
        std::cout << "Processor already initialized.\n";
        return;
    }

    std::cout << "initialize command recognized.\n";
    if (!args.empty()) {
        std::cout << "Received args: " << args << "\n";
    }

    is_initialized_ = true;
    std::cout << "\nInitialization complete.\n";

    screen_manager_.setConfig(config_);
    scheduler_ = createSchedulerFromConfig(config_, screen_manager_);
    is_initialized_ = true;

    // Start global CPU ticking thread
    ticking_ = true;
    tick_thread_ = std::thread([this]() {
        while (ticking_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            tick_count_++;
            //screen_manager_.tickAll();         // ticks all screens 
            // if (scheduler_) scheduler_->tick(); //  tick scheduler
        }
        });

}

void CLI::handleScreen(const std::string& args) {
    if (!is_initialized_) {
        std::cout << "System not initialized. Run 'initialize' first.\n";
        return;
    }

    if (args == "-ls") {
        auto screens = screen_manager_.listScreens();
        if (screens.empty()) {
            std::cout << "No active screens.\n";
        }
        else {
            std::cout << "Active screens:\n";
            for (const auto& name : screens) {
                std::cout << "- " << name << "\n";
            }
        }
        return;
    }

    char mode;
    std::string name;
    if (!parseScreenArgs(args, mode, name)) {
        std::cout << "Usage: screen -r <name> or -s <name> or -ls\n";
        return;
    }

    if (current_state_ != OsState::MAIN_MENU) {
        std::cout << "Already inside a screen. Type 'exit' to return to main menu.\n";
        return;
    }

    if (mode == 's') {
        // Use config values when creating a process
        int minIns = config_.getInt("min-ins");
        int maxIns = config_.getInt("max-ins");
        int delay = config_.getInt("delays-per-exec");

        screen_manager_.createOrFocusScreen(name, true); // can extend this to take config params later

        current_state_ = OsState::IN_SCREEN;
        active_screen_name_ = name;
        //clearScreen();
        std::cout << screen_manager_.renderScreen(name);
    }
    else if (mode == 'r') {
        if (!screen_manager_.screenExists(name)) {
            std::cout << "Screen '" << name << "' does not exist. Use 'screen -s " << name << "' to create it.\n";
            return;
        }

        current_state_ = OsState::IN_SCREEN;
        active_screen_name_ = name;
        clearScreen();
        std::cout << screen_manager_.renderScreen(name);
    }
}

bool CLI::parseScreenArgs(const std::string& args, char& mode, std::string& name) {
    std::istringstream iss(args);
    std::string flag;
    iss >> flag >> name;
    return (flag.size() == 2 && flag[0] == '-' &&
        (mode = flag[1]) && (mode == 'r' || mode == 's'));
}

void CLI::returnToMainMenu() {
    current_state_ = OsState::MAIN_MENU;
    active_screen_name_.clear();
    clearScreen();
}

void CLI::handleSchedulerTest(const std::string& args) {
    std::cout << "scheduler-test command recognized. Doing something.\n";
    if (!args.empty()) {
        std::cout << "Received args: " << args << "\n";
    }
}

void CLI::handleSchedulerStart(const std::string& args) {
    if (generating_processes_) {
        std::cout << "Scheduler already running.\n";
        return;
    }

    int freq = config_.getInt("batch-process-freq");
    int minIns = config_.getInt("min-ins");
    int maxIns = config_.getInt("max-ins");
    int delay = config_.getInt("delays-per-exec");

    generating_processes_ = true;
    generator_thread_ = std::thread([this, freq, minIns, maxIns, delay]() {
        uint64_t last_tick = tick_count_;
        while (generating_processes_) {
            if ((tick_count_ - last_tick) >= freq) {
                last_tick = tick_count_;

                // generate names p01, p02, ...
                std::ostringstream name;
                name << "p" << std::setw(2) << std::setfill('0') << process_counter_++;

                screen_manager_.createOrFocusScreen(name.str(), true);
                std::cout << "[Scheduler] Created process: " << name.str() << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // avoid busy wait
        }
        });

}

void CLI::handleSchedulerStop(const std::string& args) {
    if (!generating_processes_) {
        std::cout << "Scheduler is not running.\n";
        return;
    }

    generating_processes_ = false;
    if (generator_thread_.joinable()) {
        generator_thread_.join();
    }
    std::cout << "Scheduler stopped.\n";
}



void CLI::handleReportUtil(const std::string& args) {
    std::cout << "report-util command recognized. Doing something.\n";
    if (!args.empty()) {
        std::cout << "Received args: " << args << "\n";
    }
}

void CLI::handleExit(const std::string& args) {
    if (!args.empty() && args == "-f") {
        std::cout << "Force exiting CSOPESY CLI Emulator. Goodbye!\n";
        exit(0);
    }

    if (current_state_ == OsState::IN_SCREEN) {
        returnToMainMenu();
    }
    else {

        // Stop tick thread
        ticking_ = false;
        if (tick_thread_.joinable()) tick_thread_.join();

        // Stop generator thread
        generating_processes_ = false;
        if (generator_thread_.joinable()) generator_thread_.join();

        std::cout << "Exiting CSOPESY CLI Emulator. Goodbye!\n";
        current_state_ = OsState::EXIT;
        exit(0);
    }
}