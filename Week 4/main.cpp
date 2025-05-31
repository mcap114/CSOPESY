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

void printSeparator(int width) {
    std::cout << "+" << std::string(width - 2, '-') << "+" << std::endl;
}

void printHeader(const std::string& title, int width) {
    printSeparator(width);
    std::cout << "| " << std::left << std::setw(width - 3) << title << "|" << std::endl;
    printSeparator(width);
}

void printGpuSummary() {
    printHeader("NVIDIA-SMI 551.86               Driver Version: 551.86               CUDA Version: 12.4", 91);
    std::cout << "| GPU Name           | TCC/WDDM | Bus-Id           | Disp.A | Volatile Uncorr. ECC |" << std::endl;
    std::cout << "| Fan Temp Perf Pwr:Usage/Cap | Memory-Usage | GPU-Util | Compute M. | MIG M.     |" << std::endl;
    printSeparator(91);
    std::cout << "| 0  NVIDIA GeForce GTX 1080 | WDDM     | 00000000:26:00.0 | On     | N/A               |" << std::endl;
    std::cout << "| 28%  37C  P8    11W / 180W | 701MiB / 8192MiB | 0%       | Default    | N/A         |" << std::endl;
    printSeparator(91);
}

void printProcesses(const std::vector<ProcessInfo>& processes) {
    printHeader("Processes:", 91);
    std::cout << "| GPU  PID   Type  Process name                    GPU Memory Usage         |" << std::endl;
    printSeparator(91);
    for (const auto& p : processes) {
        std::cout << "| " << std::left << std::setw(4) << "0"
                  << std::setw(6) << p.pid
                  << std::setw(6) << p.type
                  << std::setw(35) << p.name.substr(0, 34)
                  << std::setw(25) << p.memUsage << "|" << std::endl;
    }
    printSeparator(91);
}

int main() {
    std::vector<ProcessInfo> processes = {
        {1234, "C+G", "C:\\Windows\\System32\\dwm.exe", "N/A"},
        {2345, "C+G", "C:\\Program Files\\Game\\engine.exe", "N/A"},
        {3456, "C+G", "C:\\Windows\\explorer.exe", "N/A"},
        {4567, "C+G", "C:\\AppData\\Tools\\tracker.exe", "N/A"},
        {5678, "C+G", "C:\\System\\LongNamedExecutableThatWillBeTrimmed.exe", "N/A"}
    };

    printGpuSummary();
    printProcesses(processes);

    return 0;
}
