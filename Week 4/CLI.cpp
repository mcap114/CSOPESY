#include <iostream>
#include <iomanip>
#include <string>

std::string truncateWithEllipsis(const std::string& str, size_t maxLength) {
    if (str.length() > maxLength) {
        return str.substr(0, maxLength - 3) + "...";
    }
    return str;
}

void printGpuSummary() {
    std::cout << "+-----------------------------------------------------------------------------+\n";
    std::cout << "| NVIDIA-SMI Dummy UI                                                         |\n";
    std::cout << "|-------------------------------+----------------------+----------------------+\n";
    std::cout << "| GPU  Name            Temp     | Power Usage / Max    | Memory Usage / Total |\n";
    std::cout << "|===============================+======================+======================|\n";
    std::cout << "|   0  NVIDIA GT 220      12 C  | 45W / 300W           |  1200MB / 24576MB    |\n";
    std::cout << "+-------------------------------+----------------------+----------------------+\n";
}

void printProcesses() {
    struct Process {
        int gpu;
        int pid;
        char type;
        std::string name;
        int memory;
    };

    Process processes[] = {
        {0, 1234, 'C', "steam.exe", 400},
        {0, 2345, 'S', "apex_legends.exe", 350},
        {0, 3456, 'O', "rune-elden.ring.nightreign.iso.deluxe.edition.v1.exe", 250},
        {0, 4567, 'P', "devenv.exe", 150},
        {0, 5678, 'S', "discord.exe", 50}
    };

    std::cout << "+-----------------------------------------------------------------------------+\n";
    std::cout << "| Processes:                                                                  |\n";
    std::cout << "|  GPU   PID   Type    Process name                            GPU Memory     |\n";
    std::cout << "|=============================================================================|\n";

    for (const auto& proc : processes) {
        std::cout << "| " << std::setw(4) << proc.gpu << " ";
        std::cout << std::setw(6) << proc.pid << " ";
        std::cout << std::setw(4) << proc.type << "    ";

        std::string displayName = truncateWithEllipsis(proc.name, 38);
        std::cout << std::left << std::setw(38) << displayName << " ";

        std::cout << std::right << std::setw(6) << proc.memory << "MB        |\n";
    }

    std::cout << "+-----------------------------------------------------------------------------+\n";
}

int main() {
    printGpuSummary();
    printProcesses();
    return 0;
}