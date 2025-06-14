#include "CLI.h"
#include <iostream>
#include <string>
#include <cstdlib> // for system()
#include <sstream>
#include <iomanip>

CLI::CLI() : current_state_(AppState::MAIN_MENU) {
    // commands defined in CLI.h
    commands["initialize"] = [this](const std::string& args) { handleInitialize(args); };
    commands["screen"] = [this](const std::string& args) { handleScreen(args); };
    commands["scheduler-test"] = [this](const std::string& args) { handleSchedulerTest(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["clear"] = [this](const std::string& args) { clearScreen(); };
    commands["exit"] = [this](const std::string& args) { handleExit(args); };
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
    std::cout << "initialize command recognized. Doing something.\n";
    if (!args.empty()) {
        std::cout << "Received args: " << args << "\n";
    }
}

void CLI::handleScreen(const std::string& args) {
    if (args.empty()) {
        std::cout << "Usage: screen -r <name> (running) or -s <name> (stopped)\n";
        return;
    }

    char mode;
    std::string name;
    if (!parseScreenArgs(args, mode, name)) {
        std::cout << "Invalid format. Use: screen -r <name> or -s <name>\n";
        return;
    }

    if (current_state_ == AppState::MAIN_MENU) {
        screen_manager_.createOrFocusScreen(name, mode == 'r');
        current_state_ = AppState::IN_SCREEN;
        active_screen_name_ = name;

        // Clear and redraw with prompt
        clearScreen();
        std::cout << screen_manager_.renderScreen(name) << "\n";
        std::cout << "\nEnter a command: ";  // Explicit prompt for screen mode
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

void CLI::handleSchedulerTest(const std::string& args) {
    int numProcesses = 10;
    int printsPerProcess = 100;

    // parse arguments 
    std::istringstream iss(args);
    if (!args.empty()) {
        iss >> numProcesses >> printsPerProcess;
    }

    std::cout << "Starting scheduler test with " << numProcesses
        << " processes, each with " << printsPerProcess
        << " print commands\n";

    // create and add processes
    for (int i = 1; i <= numProcesses; i++) {
        std::string name = "process" + std::to_string(i);
        scheduler_->addProcess(std::make_shared<Process>(name, printsPerProcess));
        std::cout << "Added " << name << "\n";
    }
}

void CLI::handleSchedulerStop(const std::string& args) {
    std::cout << "Stopping scheduler...\n";
    scheduler_->shutdown();
    std::cout << "Scheduler stopped\n";
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

    if (current_state_ == AppState::IN_SCREEN) {
        returnToMainMenu();
    }
    else {
        std::cout << "Exiting CSOPESY CLI Emulator. Goodbye!\n";
        exit(0);
    }
}