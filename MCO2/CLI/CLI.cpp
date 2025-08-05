#define NOMINMAX
#include "Screen.h"
#include "CLI.h"
#include <iostream>
#include <string>
#include <cstdlib> // for system()
#include <sstream>
#include <iomanip>
#include <thread>   // For sleep
#include <chrono>   // For milliseconds
#include <algorithm>
#include <random>

CLI::CLI() :
    current_state_(AppState::MAIN_MENU),
    scheduler_(std::make_unique<FCFSScheduler>(4)) 
{

    // Initialize commands
    commands["initialize"] = [this](const std::string& args) { handleInitialize(args); };
    commands["screen"] = [this](const std::string& args) { handleScreen(args); };
    commands["scheduler-start"] = [this](const std::string& args) { handleSchedulerStart(args); };
    //commands["scheduler-test"] = [this](const std::string& args) { handleSchedulerTest(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["process-smi"] = [this](const std::string& args) { handleProcessSMI(args); };
    commands["vm-stat"] = [this](const std::string& args) { handleVmstat(args); };
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
    int maxMem = config_.getInt("max-overall-mem");
    int memPerFrame = config_.getInt("mem-per-frame");
    int minMemPerProc = config_.getInt("min-mem-per-proc");
    int maxMemPerProc = config_.getInt("max-mem-per-proc");

    if ((minMemPerProc & (minMemPerProc - 1)) != 0 || (maxMem & (maxMem - 1)) != 0) {
        throw std::runtime_error("Memory values must be powers of two.");
    }

    if (minMemPerProc > maxMemPerProc) {
        throw std::runtime_error("min-mem-per-proc cannot be greater than max-mem-per-proc.");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minMemPerProc, maxMemPerProc);
    

    int rolledMem = dist(gen);

    std::string schedulerType = config_.getString("scheduler");
if (schedulerType == "fcfs") {
    std::cout << "[DEBUG] Using FCFS scheduler.\n";
} else if (schedulerType == "rr") {
    int quantum = config_.getInt("quantum-cycles");
    std::cout << "[DEBUG] Using Round-Robin scheduler with quantum " << quantum << ".\n";
} else {
    throw std::runtime_error("Unknown scheduler type: " + schedulerType);
}
    std::cout << "rolledMem value";
    std::cout << rolledMem;

    //round to nearest lower power of 2 within range
    while ((rolledMem & (rolledMem - 1)) != 0) {
        rolledMem--;  // reduce until it's a power of two
    }

    int memPerProc = rolledMem;

    cpu_timer_.start(); 
    report_gen_ = std::make_unique<ReportGenerator>(num_cores); 

    if (scheduler_type == "rr") {
        int quantum = config_.getInt("quantum-cycles");
        scheduler_ = std::make_unique<RRScheduler>(num_cores, quantum, delay, maxMem, memPerProc);
        // std::cout << "[DEBUG] Scheduler type: " << scheduler_type << "\n";
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
    commands["scheduler-start"] = [this](const std::string& args) { handleSchedulerStart(args); };
    commands["scheduler-stop"] = [this](const std::string& args) { handleSchedulerStop(args); };
    commands["report-util"] = [this](const std::string& args) { handleReportUtil(args); };
    commands["process-smi"] = [this](const std::string& args) { handleProcessSMI(args); };
    commands["vm-stat"] = [this](const std::string& args) { handleVmstat(args); };
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

    if (args.find("-c") == 0) {
        std::string name;
        int memorySize;
        std::string instructionStr;
        if (!parseScreenCustomArgs(args, name, memorySize, instructionStr)) {
            std::cout << "Usage: screen -c <name> <memory_size> \"<instruction_set>\"\n";
            return;
        }

        if (memorySize < 64 || memorySize > 216 || (memorySize & (memorySize - 1)) != 0) {
            std::cout << "Invalid memory allocation. Must be a power of 2 between 64 and 216 bytes.\n";
            return;
        }

        current_state_ = AppState::IN_SCREEN;
        active_screen_name_ = name;
        createCustomInstructionScreen(name, memorySize, instructionStr);
        clearScreen();
        std::cout << screen_manager_.renderScreen(name);
        return;
    }

    
    char mode;
    std::string name;
    int memorySize = 0;

    if (!parseScreenArgs(args, mode, name, memorySize)) {
        std::cout << "Use: screen -r <name> or -s <name> <memory_size> or -ls\n";
        return;
    }

    if (current_state_ != AppState::MAIN_MENU) {
        std::cout << "Already in a screen. Type 'exit' first.\n";
        return;
    }

    // only 'screen -s' creates new screen
    if (mode == 's') {
        // Validate memory size
        if (memorySize < 64 || memorySize > 216 || (memorySize & (memorySize - 1)) != 0) {
            std::cout << "Invalid memory allocation. Must be a power of 2 between 64 and 216 bytes.\n";
            return;
        }
    
        int totalPrints = config_.getInt("max-ins");
        createProcessScreen(name, totalPrints, memorySize); // pass memory size

        current_state_ = AppState::IN_SCREEN;
        active_screen_name_ = name;
        clearScreen();
        std::cout << screen_manager_.renderScreen(name);
    }
    else if (mode == 'r') {
        if (!screen_manager_.screenExists(name)) {
            auto proc = scheduler_->getProcess(name);
            if (proc && proc->hasMemoryViolation()) {
                std::cout << "Process " << name << " shut down due to memory access violation error that occurred at "
                        << proc->getViolationTimestamp() << ". "
                        << proc->getInvalidAddress() << " invalid.\n";
            } else {
                std::cout << "Process " << name << " not found.\n";
            }
            return;
        }
        
        current_state_ = AppState::IN_SCREEN;
        active_screen_name_ = name;
        clearScreen();
        std::cout << screen_manager_.renderScreen(name);
    }
}

bool CLI::parseScreenArgs(const std::string& args, char& mode, std::string& name, int& memorySize) {
    std::istringstream iss(args);
    std::string flag;
    iss >> flag >> name >> memorySize;
    return (flag.size() == 2 && flag[0] == '-' &&
        (mode = flag[1]) && (mode == 'r' || mode == 's'));
}

bool CLI::parseScreenCustomArgs(const std::string& args, std::string& name, int& memorySize, std::string& instructionStr) {
    std::istringstream iss(args);
    std::string flag;
    iss >> flag >> name >> memorySize;

    // Get everything after memorySize as raw instruction line
    std::getline(iss, instructionStr);

    // Trim leading/trailing whitespace
    instructionStr.erase(0, instructionStr.find_first_not_of(" \t"));
    instructionStr.erase(instructionStr.find_last_not_of(" \t") + 1);

    // Must start and end with double quotes
    size_t first_quote = instructionStr.find('"');
    size_t last_quote = instructionStr.rfind('"');

    if (first_quote == std::string::npos || last_quote == std::string::npos || last_quote <= first_quote) {
        return false;
    }

    // Extract inside the quotes
    instructionStr = instructionStr.substr(first_quote + 1, last_quote - first_quote - 1);
    return true;
}


void CLI::returnToMainMenu() {
    current_state_ = AppState::MAIN_MENU;
    active_screen_name_.clear();
    clearScreen();
}

void CLI::createProcessScreen(const std::string& processName, int totalPrints, int memorySize) {
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

    auto* rr = dynamic_cast<RRScheduler*>(scheduler_.get());
        if (rr && rr->getMemoryManager()) {
            rr->getMemoryManager()->writeToBackingStore();
        }

    screen_manager_.addProcess(processName, info);
}
void CLI::createCustomInstructionScreen(const std::string& processName, int memorySize, const std::string& instructionStr) {
    // 1. Split instruction string into vector
    std::vector<std::string> rawInstructions;
    std::istringstream stream(instructionStr);
    std::string instr;

    while (std::getline(stream, instr, ';')) {
        if (!instr.empty()) {
            rawInstructions.push_back(instr);
        }
    }

    // 2. Validate instruction count
    if (rawInstructions.size() < 1 || rawInstructions.size() > 50) {
        std::cout << "Invalid command: instruction count must be between 1 and 50.\n";
        return;
    }

    // 3. Prepare screen and process
    screen_manager_.createOrFocusScreen(processName, true);
    int procId = next_process_id_++;

    auto proc = std::make_shared<OsProcess>(processName, static_cast<int>(rawInstructions.size()));
    proc->setProcessId(procId);
    proc->parseUserInstructions(rawInstructions); // <-- we'll write this next step

    // 4. Prepare screen display info
    ProcessInfo info;
    info.id = procId;
    info.name = processName;
    info.status = "RUNNING";
    info.core = -1;
    info.progress = "0/" + std::to_string(rawInstructions.size());

    std::time_t now = std::time(nullptr);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S", std::localtime(&now));
    info.creation_time = buffer;

    info.instruction_line = 0;
    info.total_instructions = static_cast<int>(rawInstructions.size());

    // 5. Set callback for progress update
    proc->setUpdateCallback([this, proc](const std::string& pname, int coreId, const std::string& progress) {
        ProcessInfo updatedInfo;
        updatedInfo.name = pname;
        updatedInfo.core = coreId;
        updatedInfo.status = "RUNNING";
        updatedInfo.progress = progress;
        updatedInfo.instruction_line = proc->getInstructionLine();
        screen_manager_.updateProcess(pname, pname, updatedInfo);
    });

    // 6. Add to scheduler and memory manager
    scheduler_->addProcess(proc);

    if (auto* rr = dynamic_cast<RRScheduler*>(scheduler_.get())) {
        if (rr->getMemoryManager()) {
            rr->getMemoryManager()->writeToBackingStore(); // only ONCE, no stamp flood
        }
    }

    // 7. Add process to screen
    screen_manager_.addProcess(processName, info);
}

void CLI::handleSchedulerStart(const std::string& args) {
    int batchFreq = config_.getInt("batch-process-freq");
    int minInstructions = config_.getInt("min-ins");
    int maxInstructions = config_.getInt("max-ins");
    int minMem = config_.getInt("min-mem-per-proc");
    int maxMem = config_.getInt("max-mem-per-proc");

    if (minMem > maxMem) {
        throw std::runtime_error("min-mem-per-proc is greater than max-mem-per-proc.");
    }

    // Gather all valid power-of-two sizes between min and max mem
    std::vector<int> validMemSizes;
    for (int p = 6; p <= 16; ++p) {
        int size = 1 << p;
        if (size >= minMem && size <= maxMem) {
            validMemSizes.push_back(size);
        }
    }

    if (validMemSizes.empty()) {
        throw std::runtime_error("No valid memory sizes (power of two) between min and max mem per proc.");
    }

    // Setup random generators
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> memDist(0, validMemSizes.size() - 1);
    std::uniform_int_distribution<> insDist(minInstructions, maxInstructions);

    std::cout << "[INFO] Scheduler started. Generating processes every " << batchFreq << " cycles...\n";

    int processCounter = 1;

    const int totalTicks = 60;  // default
    for (int tick = 1; tick <= totalTicks; ++tick) {
        cpu_timer_.tick();  // increment

        if (tick % batchFreq == 0) {
            // create a new process
            std::string name = "proc" + std::to_string(processCounter++);
            int memSize = validMemSizes[memDist(gen)];
            int numInstructions = insDist(gen);

            createProcessScreen(name, numInstructions, memSize);

            std::cout << "[INFO] Created process " << name
                << " with " << numInstructions << " instructions and "
                << memSize << " bytes of memory.\n";
        }
    }

    std::cout << "[INFO] Scheduler finished after " << totalTicks << " ticks.\n";
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
        int memorySize = config_.getInt("max-mem-per-proc");
        createProcessScreen(name, printsPerProcess, memorySize);

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
   // scheduler_ = std::make_unique<FCFSScheduler>(4);
}

void CLI::handleReportUtil(const std::string& args) {
    std::string report = screen_manager_.listScreens(); 
    static int reportCounter = 1;
    std::string filename = "csopesy-log_" + std::to_string(reportCounter++) + ".txt";
 
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to write report to file.\n";
        return;
    }

    outFile << report;
    outFile.close();

    std::cout << "Report generated at C:/" << filename << "\n";
}

void CLI::handleProcessSMI(const std::string& args) {
    int totalMemKB = config_.getInt("max-overall-mem");
    int memPerProcKB = config_.getInt("max-mem-per-proc");

    std::vector<ProcessInfo> allProcesses;
    for (const auto& [screenName, screenPtr] : screen_manager_.getAllScreens()) {
    for (const auto& proc : screenPtr->getProcesses()) {
        allProcesses.push_back(proc);
        }
    }


    int procCount = static_cast<int>(allProcesses.size());
    int usedMemKB = procCount * memPerProcKB;
    int freeMemKB = totalMemKB - usedMemKB;

    int totalCores = config_.getInt("num-cpu");
    int usedCores = std::min(procCount, totalCores); 
    int cpuUtil = (usedCores * 100) / totalCores;

    double usedMiB = usedMemKB / 1024.0;
    double totalMiB = totalMemKB / 1024.0;
    int memoryUtil = (int)((usedMiB / totalMiB) * 100);

    std::cout << "------------------------------------------------------------\n";
    std::cout << "| PROCESS-SMI V01.00  Driver Version: 01.00                |\n";
    std::cout << "------------------------------------------------------------\n";

    std::cout << "CPU-Util:     " << cpuUtil << "%\n";
    std::cout << "Memory Usage: " << std::fixed << std::setprecision(0)
              << usedMiB << "MiB / " << totalMiB << "MiB\n";
    std::cout << "Memory Util:  " << memoryUtil << "%\n";

    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "Running processes and memory usage:\n\n";

    for (const auto& proc : allProcesses) {
        std::cout << proc.name << "    " << memPerProcKB / 1024 << "MiB\n";
    }

    std::cout << "------------------------------------------------------------\n";

}

void CLI::handleVmstat(const std::string& args) {
    int totalMem = config_.getInt("max-overall-mem");
    int memPerProc = config_.getInt("max-mem-per-proc");

    // Count active processes
    int procCount = 0;
    
    for (const auto& [screenName, screenPtr] : screen_manager_.getAllScreens()) {
        procCount += static_cast<int>(screenPtr->getProcesses().size());
    }


    int usedMem = procCount * memPerProc;
    int freeMem = totalMem - usedMem;

    unsigned long long ticks = cpu_timer_.getTicks();
    unsigned long long idleTicks = std::max(0ull, ticks / 3);  // simulate 33% idle
    unsigned long long activeTicks = ticks - idleTicks;

    std::cout << "\n======================= VMSTAT =======================\n";
    std::cout << "Total Memory        : " << totalMem << " KB\n";
    std::cout << "Used Memory         : " << usedMem << " KB\n";
    std::cout << "Free Memory         : " << freeMem << " KB\n";
    std::cout << "Idle CPU Ticks      : " << idleTicks << "\n";
    std::cout << "Active CPU Ticks    : " << activeTicks << "\n";
    std::cout << "Total CPU Ticks     : " << ticks << "\n";
    std::cout << "Pages Paged In      : " << pages_paged_in_ << "\n";
    std::cout << "Pages Paged Out     : " << pages_paged_out_ << "\n";
    std::cout << "====================================================\n";

    auto* rr = dynamic_cast<RRScheduler*>(scheduler_.get());
    if (rr) {
        auto snapshot = rr->getMemoryManager()->getMemorySnapshot();

        std::cout << "\nMemory Frame Allocation:\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "| Start Addr | Size | Status | Process          |\n";
        std::cout << "--------------------------------------------------\n";
        for (const auto& block : snapshot) {
            std::cout << "| "
                    << std::setw(10) << block.start << " | "
                    << std::setw(4)  << block.size  << " | "
                    << std::setw(6)  << (block.free ? "FREE" : "USED") << " | "
                    << std::setw(15) << block.processName << " |\n";
        }
        std::cout << "--------------------------------------------------\n";

         auto* rr = dynamic_cast<RRScheduler*>(scheduler_.get());
            if (rr && rr->getMemoryManager()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                rr->getMemoryManager()->writeToBackingStore();
            }
    }
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

