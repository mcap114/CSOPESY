#include <iostream>
#include <string>
#include <cstdlib> // for system()

#include "CLI.h"

// CLI object constructor
CLI::CLI() {

    // commands defined in CLI.h
    commands["clear"] = [this]() { clearScreen(); };          

    commands["initialize"] = [this]() { handleInitialize(); };
    commands["screen"] = [this]() { handleScreen(); };        
    commands["scheduler-test"] = [this]() { handleSchedulerTest(); };
    commands["scheduler-stop"] = [this]() { handleSchedulerStop(); };
    commands["report-util"] = [this]() { handleReportUtil(); };
    commands["exit"] = [this]() { handleExit(); };            
}

// CLI 'main body'
void CLI::run() {
    CLI::printHeader();
    std::string command;
    while (true) {
        std::getline(std::cin, command);                                //user input
        if (commands.find(command) != commands.end()) {                 //check
            commands[command]();                                        //execute
            if (command != "clear") {  
                std::cout << "\nEnter a command: ";
            }
        }
        else {
            std::cout << "Unrecognized command. Try again.\n\nEnter a command: ";
        }
    }
}


// Function to display the ASCII header
void CLI::printHeader() {
    std::string resetColor = "\033[0m";             // reset to default 
    std::string mintgreenColor = "\033[38;5;87m";  // mint green 
    std::string yellowColor = "\033[38;5;228m";   // light yellow 
    
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

// Clear screen and show header again without doubling the prompt
// changed to work on windows as well
void CLI::clearScreen() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    CLI::printHeader();  // Prints the header after clearing the screen
}


void CLI::handleInitialize() {
    std::cout << "initialize command recognized. Doing something." << std::endl;
}

void CLI::handleScreen() {
    std::cout << "screen command recognized. Doing something." << std::endl;
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

