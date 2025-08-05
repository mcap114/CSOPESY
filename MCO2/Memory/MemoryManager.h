#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <optional>
#include "PageTable.h"  // Required for PageTable references

// Paging structure
struct Frame {
    uint32_t baseAddr;             // Physical base address of the frame
    bool free;                     // Is the frame free?
    std::string processName;       // Name of owning process ("" if free)
};

class MemoryManager {
public:
    // Constructor
    MemoryManager(uint32_t totalMemory, uint32_t memPerFrame);

    // Paging memory operations
    bool allocatePages(const std::string& processName, PageTable& pageTable, uint32_t numPages);   // Allocate multiple pages
    void deallocatePages(const std::string& processName, const PageTable& pageTable);              // Free all pages for process
    std::vector<Frame> getFrameSnapshot() const;                                                   // Get current frame state
    uint32_t calculateFreeFrames() const;                                                          // Count of free frames

    // File I/O
    void writeToBackingStore(const std::string& filename = "csopesy-backing-store.txt") const;

    // General
    uint32_t getTotalMemory() const;
    uint32_t getFrameSize() const { return memPerFrame_; }             // Inline definition
    uint32_t getFrameCount() const { return static_cast<uint32_t>(frames_.size()); }

private:
    uint32_t totalMemory_;                // Total memory (in KB)
    uint32_t memPerFrame_;                // Frame size (in KB)
    std::vector<Frame> frames_;           // Paging frame table
};
