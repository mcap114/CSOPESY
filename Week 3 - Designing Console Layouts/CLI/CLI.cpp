#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <ctime>

#include "CLI.h"

// Constructor
CLI::CLI() {
    commands["clear"] = [this]() { clearScreen(); };          
    commands["initialize"] = [this]() { handleInitialize(); };
    commands["screen"] = [this]() { handleScreen(); };        
    commands["scheduler-test"] = [this]() { handleSchedulerTest(); };
    commands["scheduler-stop"] = [this]() { handleSchedulerStop(); };
    commands["report-util"] = [this]() { handleReportUtil(); };
    commands["exit"] = [this]() { handleExit(); };            
}

// Run main CLI loop
void CLI::run() {
    printHeader();
    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (commands.find(command) != commands.end()) {
            commands[command]();
            if (command != "clear") {
                std::cout << "\nEnter a command: ";
            }
        } else {
            std::cout << "Unrecognized command. Try again.\n\nEnter a command: ";
        }
    }
}

// Print header
void CLI::printHeader() {
    std::string resetColor = "\033[0m";
    std::string mintgreenColor = "\033[38;5;87m";
    std::string yellowColor = "\033[38;5;228m";

    std::cout << R"(
    .------..------..------..------..------..------..------.
    |C.--. ||S.--. ||O.--. ||P.--. ||E.--. ||S.--. ||Y.--. |
    | :/\: || :/\: || :/\: || :/\: || (\/) || :/\: || (\/) |
    | :\/: || :\/: || :\/: || (__) || :\/: || :\/: || :\/: |
    | '--'C|| '--'S|| '--'O|| '--'P|| '--'E|| '--'S|| '--'Y|
    `------'`------'`------'`------'`------'`------'`------'

    )" << std::endl;

    std::cout << mintgreenColor << "Hello, Welcome to CSOPESY commandline!\n" << resetColor;
    std::cout << yellowColor << "Type 'exit' to quit, 'clear' to clear the screen\n" << resetColor;
    std::cout << "Enter a command: ";  
}

// Clear consoles when screen command is typed
// changed to work on windows as well
void CLI::clearConsole() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
}

// Clear console and show header again without doubling the prompt
// when typed in 'Enter a command: clear'
void CLI::clearScreen() {
    clearConsole();
    printHeader();
}

void CLI::handleInitialize() {
    std::cout << "initialize command recognized. Doing something." << std::endl;
}
void CLI::handleSchedulerTest() {
    std::cout << "scheduler-test command recognized. Doing something." << std::endl;
}
void CLI::handleSchedulerStop() {
    std::cout << "scheduler-stop command recognized. Doing something." << std::endl;
}
void CLI::handleReportUtil() {
    std::cout << "report-util command recognized. Doing something." << std::endl;
}
void CLI::handleExit() {
    std::cout << "Exiting CSOPESY CLI Emulator. Goodbye!" << std::endl;
    exit(0);
}

// Timestamp
std::string CLI::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm* tmPtr = std::localtime(&timeNow);
    std::ostringstream oss;
    oss << std::put_time(tmPtr, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}

// Handle 'screen' command
void CLI::handleScreen() {
    std::cout << "screen command recognized. Doing something.\n" << std::endl;
    
    std::string input;
    std::cout << "Enter screen command (e.g., screen -s myproc or screen -r myproc): ";
    std::getline(std::cin, input);

    std::istringstream iss(input);
    std::string command, option, name;
    iss >> command >> option >> name;

    if ((option == "-s" || option == "-r") && !name.empty()) {
        createOrResumeScreen(name, option == "-r");
    } else {
        std::cout << "Invalid screen command format.\n";
    }
}

// Create/resume screen
void CLI::createOrResumeScreen(const std::string& name, bool resume) {
    if (resume) {
        if (screens.find(name) != screens.end()) {
            runScreenConsole(screens[name]);
        } else {
            std::cout << "No screen found with name '" << name << "'.\n";
        }
    } else {
        if (screens.find(name) != screens.end()) {
            std::cout << "Screen with name '" << name << "' already exists.\n";
            return;
        }

        Screen newScreen = {
            name,
            1,              // Placeholder
            10,             // Placeholder
            getCurrentTimestamp()
        };
        screens[name] = newScreen;
        runScreenConsole(newScreen);
    }
}

// Simulated screen console
void CLI::runScreenConsole(const Screen& screenData) {
    clearConsole();

    Screen screen = screenData;  // Copy so we can update progress if needed
    std::string input;

    while (true) {
        clearConsole();

        int percent = (100 * screen.currentLine) / screen.totalLines;
        int barWidth = 20;
        int fill = (barWidth * percent) / 100;

        std::string progressBar = "[";
        for (int i = 0; i < barWidth; ++i) {
            progressBar += (i < fill) ? "#" : "-";
        }
        progressBar += "] " + std::to_string(percent) + "%";

        // Compose progress line for alignment
        std::stringstream progressLine;
        progressLine << screen.currentLine << " / " << screen.totalLines << " instructions";

        // Draw table
        std::cout << "+-----------+-----------------------------+\n";
        std::cout << "| Process   | " << std::setw(27) << std::left << screen.name << "|\n";
        std::cout << "+-----------+-----------------------------+\n";
        std::cout << "| Progress  | " << std::setw(27) << std::left << progressLine.str() << "|\n";
        std::cout << "| Created   | " << std::setw(27) << std::left << screen.timestamp << "|\n";
        std::cout << "+-----------+-----------------------------+\n\n";

        std::cout << "Progress: " << progressBar << "\n";
        std::cout << "Commands: 'exit' to return | 'next' to simulate progress\n\n";

        std::cout << screen.name << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            clearScreen();
            break;
        } else if (input == "next") {
            if (screen.currentLine < screen.totalLines) {
                screen.currentLine++;
            } else {
                std::cout << "Instruction progress is already complete.\n";
                std::cout << "Press Enter to continue...";
                std::getline(std::cin, input);
            }
        } else {
            std::cout << "Unknown command.\n";
            std::cout << "Press Enter to continue...";
            std::getline(std::cin, input);
        }
    }
}