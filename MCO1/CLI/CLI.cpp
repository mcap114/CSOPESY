#include "CLI.h"
#include <iostream>
#include <string>
#include <cstdlib> // for system()
#include <sstream>
#include <iomanip>

CLI::CLI() :
    current_state_(AppState::MAIN_MENU),
    scheduler_(std::make_unique<FCFSScheduler>(4)) {

    // Initialize commands
    commands["initialize"] = [this](const std::string& args) { handleInitialize(args); };
    commands["screen"] = [this](const std::string& args) { handleScreen(args); };
    commands["scheduler-test"] = [this](const std::string& args) { handleSchedulerTest(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["clear"] = [this](const std::string& args) { clearScreen(); };
    commands["exit"] = [this](const std::string& args) { handleExit(args); };
    commands["sleep"] = [this](const std::string& args) { handleSleepCommand(args); };
    commands["for"] = [this](const std::string& args) { handleForCommand(args); };
}

CLI::~CLI() {
    scheduler_->shutdown();
}

void CLI::run() {
    printHeader();
    std::string input;

    while (true) {
        if (current_state_ == AppState::MAIN_MENU) {
            std::cout << "\nEnter a command: ";
        }
        std::getline(std::cin, input);

        // Handle exit command differently based on state
        if (current_state_ == AppState::IN_SCREEN && input == "exit") {
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

    if (show_prompt && current_state_ == AppState::MAIN_MENU) {
        std::cout << "\nEnter a command: ";
    }
}

void CLI::clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
    printHeader(false);
}

void CLI::handleInitialize(const std::string& args) {
    std::cout << "initialize command recognized. Doing something.\n";
    if (!args.empty()) {
        std::cout << "Received args: " << args << "\n";
    }
}

void CLI::handleScreen(const std::string& args) {
    if (args == "-ls") {
       
        std::cout << screen_manager_.listScreens();
        return;
    }
    
    char mode;
    std::string name;
    if (!parseScreenArgs(args, mode, name)) {
        std::cout << "Use: screen -r <name> or -s <name> or -ls\n";
        return;
    }

    if (current_state_ == AppState::MAIN_MENU) {
        screen_manager_.createOrFocusScreen(name, mode == 'r');
        current_state_ = AppState::IN_SCREEN;
        active_screen_name_ = name;

        clearScreen();
        std::cout << screen_manager_.renderScreen(name) << "\n";
        std::cout << "\nEnter a command: ";
    }
    else {
        std::cout << "Already in a screen. Type 'exit' first.\n";
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
    current_state_ = AppState::MAIN_MENU;
    active_screen_name_.clear();
    clearScreen();
}
void CLI::createProcessScreen(const std::string& processName, int totalPrints) {
    // Create or focus the screen
    screen_manager_.createOrFocusScreen(processName, true);

    // Add process info to the screen
    ProcessInfo info;
    info.name = processName;
    info.status = "RUNNING";
    info.core = -1; // Will be updated when assigned
    info.progress = "0/" + std::to_string(totalPrints);
    info.creation_time = ""; // Will auto-populate

    screen_manager_.addProcess(processName, info);
}

void CLI::handleSchedulerTest(const std::string& args) {
    int numProcesses = 10;
    int printsPerProcess = 100;

    // Parse arguments if provided
    std::istringstream iss(args);
    if (!args.empty()) {
        iss >> numProcesses >> printsPerProcess;
    }

    std::cout << "Creating " << numProcesses << " processes...\n";

    for (int i = 1; i <= numProcesses; i++) {
        std::string name = "process" + std::to_string(i);

        // 1. Create the process
        scheduler_->addProcess(std::make_shared<Process>(name, printsPerProcess));

        // 2. Auto-create its screen
        createProcessScreen(name, printsPerProcess);

        std::cout << "Created process: " << name << "\n";
    }
}

void CLI::handleSchedulerStop(const std::string& args) {
    std::cout << "Stopping scheduler...\n";
    scheduler_->shutdown();
    std::cout << "Scheduler stopped\n";
    // reinitialize scheduler for future use
    scheduler_ = std::make_unique<FCFSScheduler>(4);
}

void CLI::handleReportUtil(const std::string& args) {
    std::cout << "Scheduler Utilization Report:\n";
    std::cout << "---------------------------\n";
    // TODO: add actual utilization reporting logic here
    std::cout << "Processes completed: [implement reporting]\n";
    std::cout << "Current queue size: [implement reporting]\n";
}

void CLI::handleExit(const std::string& args) {
    if (!args.empty() && args == "-f") {
        std::cout << "Force exiting CSOPESY CLI Emulator. Goodbye!\n";
        exit(0);
    }

    if (current_state_ == AppState::IN_SCREEN) {
        returnToMainMenu();
    }
    else {
        std::cout << "Exiting CSOPESY CLI Emulator. Goodbye!\n";
        exit(0);
    }
}

void CLI::handleSleepCommand(const std::string& args) {
    std::istringstream iss(args);
    std::string name;
    unsigned long ticks;
    iss >> name >> ticks;

    if (name.empty() || iss.fail()) {
        std::cout << "Usage: sleep <processName> <ticks>\n";
        return;
    }

    auto process = scheduler_->findProcess(name);
    if (process) {
        process->handleSleep(ticks);
        std::cout << "Process " << name << " is now sleeping for " << ticks << " ticks.\n";
    } else {
        std::cout << "Process not found: " << name << "\n";
    }
}

void CLI::handleForCommand(const std::string& args) {
    std::istringstream iss(args);
    std::string name;
    int iterations;
    std::string cmdList;

    iss >> name >> iterations;
    std::getline(iss, cmdList);

    if (name.empty() || iterations <= 0 || cmdList.empty()) {
        std::cout << "Usage: for <processName> <iterations> <cmd1;cmd2;...>\n";
        return;
    }

    // Remove leading space from cmdList
    if (!cmdList.empty() && cmdList[0] == ' ')
        cmdList.erase(0, 1);

    std::vector<std::string> commands;
    std::stringstream ss(cmdList);
    std::string cmd;
    while (std::getline(ss, cmd, ';')) {
        commands.push_back(cmd);
    }

    auto process = scheduler_->findProcess(name);
    if (process) {
        process->handleForLoop(commands, iterations);
        std::cout << "Process " << name << " will repeat commands for " << iterations << " iterations.\n";
    } else {
        std::cout << "Process not found: " << name << "\n";
    }
}
