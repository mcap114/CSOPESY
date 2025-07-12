#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../Scheduler/OSProcess.h"

struct MemoryBlock {
    int startAddress;
    int size;
    std::shared_ptr<OsProcess> owner;
};

class MemoryManager {
public:
    MemoryManager(int totalMemory, int memPerProc);

    bool allocateProcess(std::shared_ptr<OsProcess> process);
    void deallocateProcess(std::shared_ptr<OsProcess> process);
    void snapshot(int quantumCycle) const;

private:
    int totalMemory;
    int memPerProc;
    std::vector<MemoryBlock> memoryBlocks;

    int calculateExternalFragmentation() const;
};
