#include "MemoryManager.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

MemoryManager::MemoryManager(int totalMemory, int memPerProc)
    : totalMemory(totalMemory), memPerProc(memPerProc) {
}

bool MemoryManager::allocateProcess(std::shared_ptr<OsProcess> process) {
    // First-fit search
    int currentAddress = 0;
    for (const auto& block : memoryBlocks) {
        int gap = block.startAddress - currentAddress;
        if (gap >= memPerProc) {
            // Allocate here
            memoryBlocks.insert(memoryBlocks.begin() + (&block - &memoryBlocks[0]), { currentAddress, memPerProc, process });
            return true;
        }
        currentAddress = block.startAddress + block.size;
    }

    // Check if space at the end
    if (totalMemory - currentAddress >= memPerProc) {
        memoryBlocks.push_back({ currentAddress, memPerProc, process });
        return true;
    }

    // Not enough memory
    return false;
}

void MemoryManager::deallocateProcess(std::shared_ptr<OsProcess> process) {
    for (auto it = memoryBlocks.begin(); it != memoryBlocks.end(); ++it) {
        if (it->owner == process) {
            memoryBlocks.erase(it);
            return;
        }
    }
}

int MemoryManager::calculateExternalFragmentation() const {
    int totalFragmentation = 0;
    int currentAddress = 0;

    for (const auto& block : memoryBlocks) {
        int gap = block.startAddress - currentAddress;
        if (gap > 0) totalFragmentation += gap;
        currentAddress = block.startAddress + block.size;
    }

    // Check end gap
    if (currentAddress < totalMemory) {
        totalFragmentation += totalMemory - currentAddress;
    }

    return totalFragmentation;
}

void MemoryManager::snapshot(int quantumCycle) const {
    std::ofstream out("memory_stamp_" + std::to_string(quantumCycle) + ".txt");

    // Timestamp
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    out << "Timestamp: " << std::put_time(std::localtime(&in_time_t), "%m/%d/%Y %I:%M:%S%p") << "\n";

    out << "Number of processes in memory: " << memoryBlocks.size() << "\n";
    out << "Total external fragmentation in KB: " << calculateExternalFragmentation() << "\n\n";

    out << "----end----- = " << totalMemory << "\n";

    for (const auto& block : memoryBlocks) {
        out << block.owner->getName() << "\n";
        out << block.startAddress << "\n";
    }

    out << "----start---- = 0\n";
    out.close();
}
