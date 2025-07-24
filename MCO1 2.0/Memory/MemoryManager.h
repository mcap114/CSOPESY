#pragma once
#include <vector>
#include <cstdint>
#include <string>

struct MemoryBlock {
    uint32_t start;
    uint32_t size;
    bool free;
    std::string processName;
};

class MemoryManager {
public:
    MemoryManager(uint32_t totalMemory, uint32_t memPerProc);


    bool allocate(const std::string& processName);
    void deallocate(const std::string& processName);
    uint32_t getTotalMemory();
    uint32_t calculateExternalFragmentation() const;
    std::vector<MemoryBlock> getMemorySnapshot() const;

private:
    uint32_t totalMemory_;
    uint32_t memPerProc_;
    std::vector<MemoryBlock> blocks_;
};
