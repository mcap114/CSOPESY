#include "CLI.h"
#include <iostream>
#include <string>
#include <cstdlib> // for system()
#include <sstream>
#include <iomanip>
#include <thread>   // For sleep
#include <chrono>   // For milliseconds


CLI::CLI() :
    current_state_(AppState::MAIN_MENU),
    scheduler_(std::make_unique<FCFSScheduler>(4)) 
{

    // Initialize commands
    commands["initialize"] = [this](const std::string& args) { handleInitialize(args); };
    commands["screen"] = [this](const std::string& args) { handleScreen(args); };
    commands["scheduler-start"] = [this](const std::string& args) { handleSchedulerTest(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["clear"] = [this](const std::string& args) { clearScreen(); };
    commands["exit"] = [this](const std::string& args) { handleExit(args); };

    commands["sleep"] = [this](const std::string& args) { handleSleep(args); };
    commands["for"] = [this](const std::string& args) { handleFor(args); };
}

//constructor with config
CLI::CLI(const Config& config)
    : current_state_(AppState::MAIN_MENU), config_(config)
{
    std::string scheduler_type = config_.getString("scheduler");
    int num_cores = config_.getInt("num-cpu");
    int delay = config_.getInt("delay-per-exec");
    int quantum = config.getInt("quantum-cycles");

    cpu_timer_.start(); 
    report_gen_ = std::make_unique<ReportGenerator>(num_cores); 

    if (scheduler_type == "rr") {
        int quantum = config_.getInt("quantum-cycles");
        scheduler_ = std::make_unique<RRScheduler>(num_cores, quantum);
    }
    else if (scheduler_type == "fcfs") {
        scheduler_ = std::make_unique<FCFSScheduler>(num_cores,delay);
    }
    else {
        throw std::runtime_error("Unknown scheduler type in config: " + scheduler_type);
    }

    // Initialize commands
    commands["initialize"] = [this](const std::string& args) { handleInitialize(args); };
    commands["screen"] = [this](const std::string& args) { handleScreen(args); };
    commands["scheduler-start"] = [this](const std::string& args) { handleSchedulerTest(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["clear"] = [this](const std::string& args) { clearScreen(); };
    commands["exit"] = [this](const std::string& args) { handleExit(args); };
    commands["sleep"] = [this](const std::string& args) { handleSleep(args); };
    commands["for"] = [this](const std::string& args) { handleFor(args); };
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
        if (current_state_ == AppState::IN_SCREEN) {
            if (input == "exit") {
                returnToMainMenu();
                continue;
            }
            else if (input == "process-smi") {
                auto proc = scheduler_->getProcess(active_screen_name_);
                auto screen = screen_manager_.getScreen(active_screen_name_);
                if (proc && screen) {
                    // get process info from screen
                    const auto& procs = screen->getProcesses();
                    auto it = std::find_if(procs.begin(), procs.end(), [&](const ProcessInfo& p) {
                        return p.name == active_screen_name_;
                    });
                    if (it != procs.end()) {
                        std::cout << "\nProcess Name: " << it->name << "\n";
                        std::cout << "Process ID: " << it->id << "\n";
                        
                        std::cout << "Core #: " << it->core << "\n";
                        std::cout << "Logs:\n";
                        for (const auto& log : proc->getLogs()) {
                            std::cout << log << "\n";
                        }

                        std::cout << "\nCurrent Instruction Line: " << it->instruction_line << "\n";
                        std::cout << "Lines of Code: " << it->total_instructions << "\n";
                    } else {
                        std::cout << "Process info not found in screen.\n";
                    }
                } else {
                    std::cout << "Process or screen not found.\n";
                }
                std::cout << "\nroot:\\> ";
            }
            else {
                std::cout << "Unrecognized command in screen. Type 'process-smi' or 'exit'.\n";
                std::cout << "root:\\> ";
            }
            continue;
        }

        // Split into command and arguments
        auto space_pos = input.find(' ');
        std::string cmd = input.substr(0, space_pos);
        std::string args = (space_pos != std::string::npos) ? input.substr(space_pos + 1) : "";

        // If not initialized
        if (commands.find(cmd) != commands.end()) {
            if (!is_initialized_ && cmd != "initialize" && cmd != "exit") {
                std::cout << "System not initialized. Please run 'initialize' first.\n";
                continue;
            }

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
    if (current_state_ == AppState::MAIN_MENU) {
        printHeader();
    }
}

void CLI::handleInitialize(const std::string& args) {
    if (is_initialized_) {
        std::cout << "Processor already initialized.\n";
        return;
    }

    std::cout << "initialize command recognized. Doing something.\n";
    if (!args.empty()) {
        std::cout << "Received args: " << args << "\n";
    }

    is_initialized_ = true;
    std::cout << "\nInitialization complete.\n";
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

    if (current_state_ != AppState::MAIN_MENU) {
        std::cout << "Already in a screen. Type 'exit' first.\n";
        return;
    }

    // only 'screen -s' creates new screen
    if (mode == 's') {
        
        int totalPrints = config_.getInt("max-ins");
        createProcessScreen(name, totalPrints);

        current_state_ = AppState::IN_SCREEN;
        active_screen_name_ = name;
        clearScreen();
        std::cout << screen_manager_.renderScreen(name);
    }
    else if (mode == 'r') {
        if (!screen_manager_.screenExists(name)) {
            std::cout << "Process " << name << " not found. Need to be initialized via screen -s " << name << "\n";
            return;
        }
        
        current_state_ = AppState::IN_SCREEN;
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
    current_state_ = AppState::MAIN_MENU;
    active_screen_name_.clear();
    clearScreen();
}

void CLI::createProcessScreen(const std::string& processName, int totalPrints) {
    // Create or focus the screen
    screen_manager_.createOrFocusScreen(processName, true);

    int procId = next_process_id_++;

    int minIns = config_.getInt("min-ins");            // [CONFIG UPDATE]
    int maxIns = config_.getInt("max-ins");            // [CONFIG UPDATE]
    int delay = config_.getInt("delay-per-exec");      // [CONFIG UPDATE]
    int instructionCount = minIns + (rand() % (maxIns - minIns + 1));

    auto proc = std::make_shared<OsProcess>(processName, instructionCount);
    proc->setProcessId(procId);  

    //int instructionCount = rand() % 6 + 5; // random between 5 and 10
    proc->generateRandomInstructions(instructionCount);

    // Add process info to the screen
    ProcessInfo info;
    info.id = procId;
    info.name = processName;
    info.status = "RUNNING";
    info.core = -1; // Will be updated when assigned
    info.progress = "0/" + std::to_string(totalPrints);
    
    std::time_t now = std::time(nullptr);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S", std::localtime(&now));
    info.creation_time = buffer;

    info.instruction_line = 0;
    info.total_instructions = totalPrints;

    proc->setUpdateCallback([this, proc](const std::string& pname, int coreId, const std::string& progress) {
        ProcessInfo updatedInfo;
        updatedInfo.name = pname;
        updatedInfo.core = coreId;
        updatedInfo.status = "RUNNING";
        updatedInfo.progress = progress;
        updatedInfo.instruction_line = proc->getInstructionLine();  // NEW
        screen_manager_.updateProcess(pname, pname, updatedInfo);
    });
    

    scheduler_->addProcess(proc);

    screen_manager_.addProcess(processName, info);
}

void CLI::handleSchedulerTest(const std::string& args) {
    int numProcesses = 10;
    int printsPerProcess = 100;
    int freq = config_.getInt("batch-process-freq");

    std::istringstream iss(args);
    if (!args.empty()) {
        iss >> numProcesses >> printsPerProcess;
    }

    std::cout << "Creating " << numProcesses << " processes...\n";

    for (int i = 1; i <= numProcesses; i++) {
        std::string name = "process" + std::to_string(i);
        createProcessScreen(name, printsPerProcess);

        // Simulate delay between process creation
        for (int j = 0; j < freq; ++j) {
            cpu_timer_.tick();  // [ADDED]
        }
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
    is_initialized_ = false;

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

void CLI::handleSleep(const std::string& args) {
    try {
        int duration_ms = std::stoi(args);
        std::cout << "Sleeping for " << duration_ms << "ms...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
        std::cout << "Awake.\n";
    } catch (...) {
        std::cout << "Invalid usage. Try: sleep 1000\n";
    }
}

void CLI::handleFor(const std::string& args) {
    try {
        int count = std::stoi(args);
        for (int i = 1; i <= count; ++i) {
            std::cout << "Iteration " << i << " of " << count << "\n";
        }
    } catch (...) {
        std::cout << "Invalid usage. Try: for 5\n";
    }
}