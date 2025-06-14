#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


struct Process {
    int gpu;
    int pid;
    char type;
    std::string name;
    int memory;
};


struct ColumnWidths {
    int gpu = 4;
    int pid = 6;
    int type = 6;
    int name = 30;  
    int memory = 10;
};


int calculateMaxWidth(const std::vector<Process>& processes, int Process::* field) {
    int max_width = 0;
    for (const auto& proc : processes) {
        int length = std::to_string(proc.*field).length();
        if (length > max_width) max_width = length;
    }
    return max_width;
}

void printLine(int total_width) {
    std::cout << "+";
    for (int i = 0; i < total_width - 2; ++i) std::cout << "-";
    std::cout << "+\n";
}


void printGpuSummary() {
    std::string gpuName = "NVIDIA GT 220";
    std::string temp = "42 C";
    std::string power = "45W / 300W";
    std::string memory = "1200MB / 24576MB";

    int total_width = 80;  
    printLine(total_width);


    std::cout << "| NVIDIA-SMI Dummy UI";
    std::cout << std::string(total_width - 22 - 1, ' ') << "|\n";

    // GPU Info
    std::cout << "| GPU  " << gpuName << "  " << temp;
    std::cout << "  |  " << power;
    std::cout << "  |  " << memory;
    std::cout << std::string(total_width - gpuName.length() - temp.length() - power.length() - memory.length() - 16, ' ') << "|\n";

    printLine(total_width);
}

void printProcesses(const std::vector<Process>& processes) {
    ColumnWidths widths;
    widths.gpu = std::max(widths.gpu, calculateMaxWidth(processes, &Process::gpu));
    widths.pid = std::max(widths.pid, calculateMaxWidth(processes, &Process::pid));
    widths.memory = std::max(widths.memory, calculateMaxWidth(processes, &Process::memory));

    for (const auto& proc : processes) {
        widths.name = std::max(widths.name, static_cast<int>(proc.name.length()));
    }
    widths.name = std::min(widths.name, 40);  // Cap at 40 chars

    // Total table width
    int total_width = widths.gpu + widths.pid + widths.type + widths.name + widths.memory + 10; // +10 for spacing/borders

    printLine(total_width);
    std::cout << "| Processes:" << std::string(total_width - 12 - 1, ' ') << "|\n";

    // Header
    std::cout << "| GPU" << std::string(widths.gpu - 3, ' ');
    std::cout << " PID" << std::string(widths.pid - 3, ' ');
    std::cout << " Type ";
    std::cout << " Process Name" << std::string(widths.name - 12, ' ');
    std::cout << " GPU Memory  |\n";

    printLine(total_width);

    for (const auto& proc : processes) {
        std::cout << "| " << proc.gpu << std::string(widths.gpu - std::to_string(proc.gpu).length(), ' ');
        std::cout << " " << proc.pid << std::string(widths.pid - std::to_string(proc.pid).length(), ' ');
        std::cout << " " << proc.type << "    ";

        std::string displayName = proc.name;
        if (displayName.length() > widths.name) {
            displayName = displayName.substr(0, widths.name - 3) + "...";
        }
        std::cout << displayName << std::string(widths.name - displayName.length(), ' ');

        std::cout << " " << proc.memory << "MB" << std::string(widths.memory - std::to_string(proc.memory).length() - 2, ' ') << " |\n";
    }

    printLine(total_width);
}

int main() {
    std::vector<Process> processes = {
        {0, 1234, 'C', "steam.exe", 400},
        {0, 2345, 'C', "apex_legends.exe", 350},
        {0, 3456, 'C', "rune-elden.ring.nightreign.iso.deluxe.edition.v1.exe", 250},
        {0, 4567, 'G', "devenv.exe", 150},
        {0, 5678, 'C', "discord.exe", 50}
    };

    printGpuSummary();
    printProcesses(processes);

    return 0;
}