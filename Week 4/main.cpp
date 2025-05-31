#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string type;
    std::string name;
    std::string memUsage;
};

void printLine(int width = 119) {
    std::cout << "+" << std::string(width - 2, '-') << "+" << std::endl;
}

void printGpuSummary() {
    printLine();
    std::cout << "| NVIDIA-SMI 551.86                   | Driver Version: 551.86                     | CUDA Version: 12.4               |\n";
    printLine();
    std::cout << "| GPU  Name                           | TCC/WDDM | Bus-Id        | Disp.A          | Volatile Uncorr. ECC |\n";
    std::cout << "| Fan  Temp Perf Pwr:Usage/Cap        | Memory-Usage             | GPU-Util        | Compute M.           | MIG M.    |\n";
    printLine();
    std::cout << "| 0    NVIDIA GeForce GTX 1080        | WDDM                     | 0000:26:00.0 On | N/A                  |\n";
    std::cout << "| 28%  37C  P8   11W / 180W           | 701MiB / 8192MiB         | 0%              | Default              | N/A       |\n";
    printLine();
}

void printProcesses(const std::vector<ProcessInfo>& processes) {
    printLine();
    std::cout << "| Processes:                                                                                                  |\n";
    printLine();
    std::cout << "| GPU | PID   | Type | Process name                                  | GPU Memory Usage |\n";
    printLine();

    for (const auto& p : processes) {
        std::string displayName = p.name;
        if (p.name.length() > 45) {
            size_t lastSlash = p.name.find_last_of("\\");
            if (lastSlash != std::string::npos && lastSlash < 30) {
                std::string baseName = p.name.substr(lastSlash + 1);
                displayName = p.name.substr(0, lastSlash + 1) + "..." + 
                             baseName.substr(baseName.length() - (45 - (lastSlash + 1 + 3)));
            } else {
                displayName = "..." + p.name.substr(p.name.length() - 42);
            }
        }

        std::cout << "| " 
                  << std::setw(3) << 0 << " | "
                  << std::setw(5) << p.pid << " | "
                  << std::setw(4) << p.type << " | "
                  << std::setw(45) << std::left << displayName << " | "
                  << std::setw(16) << std::left << p.memUsage << " |\n";
    }

    printLine();
}

int main() {
    std::vector<ProcessInfo> processes = {
        {1234, "C+G", "C:\\Windows\\System32\\dwm.exe", "N/A"},
        {2245, "C+G", "C:\\Program Files\\Game\\engine.exe", "N/A"},
        {3456, "C+G", "C:\\Windows\\explorer.exe", "N/A"},
        {4567, "C+G", "C:\\AppData\\Tools\\tracker.exe", "N/A"},
        {5678, "C+G", "C:\\System\\LongNamedExecutableThatWillBeTrimmed.exe", "N/A"}
    };

    printGpuSummary();
    printProcesses(processes);

    return 0;
}