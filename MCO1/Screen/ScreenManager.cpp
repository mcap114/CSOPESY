#include "ScreenManager.h"

void ScreenManager::createOrFocusScreen(const std::string& name, bool is_running) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    auto it = screens_.find(name);
    if (it != screens_.end()) {
        it->second->setRunning(is_running);  
    }
    else {
        screens_[name] = std::make_unique<Screen>(name, is_running);
    }
}

void ScreenManager::removeScreen(const std::string& name) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    screens_.erase(name);
}

bool ScreenManager::screenExists(const std::string& name) const {
    // Check if the screen exists in the screens_ map
    return screens_.find(name) != screens_.end();
}

std::string ScreenManager::renderScreen(const std::string& name) const {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    auto it = screens_.find(name);
    return (it != screens_.end()) ? it->second->render() : "Screen not found";
}

void ScreenManager::setRunning(const std::string& name, bool running) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    if (auto it = screens_.find(name); it != screens_.end()) {
        it->second->setRunning(running);  
    }
}

void ScreenManager::addProcess(const std::string& screen_name,
    const ProcessInfo& process) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    if (auto it = screens_.find(screen_name); it != screens_.end()) {
        it->second->addProcess(process);
    }
}

void ScreenManager::updateProcess(const std::string& screen_name, const std::string& process_name,
    const ProcessInfo& updated) {
        std::lock_guard<std::mutex> lock(screen_mutex_);
        if (auto it = screens_.find(screen_name); it != screens_.end()) {
            auto& procs = it->second->getProcessesRef(); // NEW FUNCTION you'll add next
            for (auto& proc : procs) {
                if (proc.name == process_name) {
                    proc.status = updated.status;
                    proc.core = updated.core;
                    proc.progress = updated.progress;
                    proc.instruction_line = updated.instruction_line;
                    break;
                }
            }
        }    
}

void ScreenManager::removeProcess(const std::string& screen_name, const std::string& process_name) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    if (auto it = screens_.find(screen_name); it != screens_.end()) {
        it->second->removeProcess(process_name);
    }
}

std::string ScreenManager::listScreens() const {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    std::ostringstream oss;

    // Placeholder CPU stats (customize later if needed)
    int total_cores = 4;
    int used_cores = 4;
    int available_cores = total_cores - used_cores;
    int utilization = (used_cores * 100) / total_cores;

    oss << "CPU utilization: " << utilization << "%\n";
    oss << "Cores used: " << used_cores << "\n";
    oss << "Cores available: " << available_cores << "\n\n";

    // Split processes into running and finished
    std::vector<ProcessInfo> running;
    std::vector<ProcessInfo> finished;

    for (const auto& [name, screen] : screens_) {
        const auto& processes = screen->getProcesses();
        if (!processes.empty()) {
            const auto& proc = processes[0];
            if (proc.instruction_line >= proc.total_instructions) {
                finished.push_back(proc);
            } else {
                running.push_back(proc);
            }
        }
    }

    // Running section
    oss << "------------------------------------------------------------\n";
    oss << "Running processes:\n";
    for (const auto& proc : running) {
        oss << proc.name
            << "\t(" << proc.creation_time << ")"
            << "\tCore: " << proc.core
            << "\t" << proc.instruction_line << " / " << proc.total_instructions << "\n";
    }

    // Finished section
    oss << "\nFinished processes:\n";
    for (const auto& proc : finished) {
        oss << proc.name
            << "\t(" << proc.creation_time << ")"
            << "\tFinished"
            << "\t" << proc.total_instructions << " / " << proc.total_instructions << "\n";
    }

    return oss.str();
}


Screen* ScreenManager::getScreen(const std::string& name) {
    std::lock_guard<std::mutex> lock(screen_mutex_);
    auto it = screens_.find(name);
    if (it != screens_.end()) {
        return it->second.get();
    }
    return nullptr;
}