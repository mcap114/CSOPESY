#include "MemoryManager.h"
#include <fstream> 
#include <iostream>
#include <optional>
#include <algorithm>

MemoryManager::MemoryManager(uint32_t totalMemory, uint32_t memPerFrame)
    : totalMemory_(totalMemory), memPerFrame_(memPerFrame) {

    std::ofstream log("allocation-debug.log", std::ios::app);
    log << "[MEMMAN] MemoryManager constructor called\n";
    log.close();

    uint32_t numFrames = totalMemory_ / memPerFrame_;
    for (uint32_t i = 0; i < numFrames; ++i) {
        frames_.push_back({ i * memPerFrame_, true, "" });
    }
}

bool MemoryManager::allocatePages(const std::string& processName, PageTable& pageTable, uint32_t numPages) {
    std::ofstream log("allocation-debug.log", std::ios::app);
    log << "[ALLOC] Allocating " << numPages << " pages for process " << processName << "\n";

    uint32_t allocated = 0;
    for (auto& frame : frames_) {
        if (frame.free) {
            frame.free = false;
            frame.processName = processName;
            pageTable.setFrame(allocated, frame.baseAddr);
            allocated++;
            if (allocated == numPages) break;
        }
    }

    if (allocated < numPages) {
        log << "[ALLOC] Not enough memory to allocate all pages for " << processName << "\n";
        log.close();
        return false;
    }

    log << "[ALLOC] Allocation SUCCESS\n";
    log.close();
    return true;
}

void MemoryManager::deallocatePages(const std::string& processName, const PageTable& pageTable) {
    std::ofstream log("allocation-debug.log", std::ios::app);
    log << "[DEALLOC] Deallocating pages for process: " << processName << "\n";

    for (const auto& entry : pageTable.getTable()) {
        for (auto& frame : frames_) {
            if (!frame.free && frame.baseAddr == entry.frameAddr && frame.processName == processName) {
                frame.free = true;
                frame.processName = "";
                break;
            }
        }
    }

    log << "[DEALLOC] Deallocation complete.\n";
    log.close();
}

uint32_t MemoryManager::getTotalMemory() const {
    return totalMemory_;
}

uint32_t MemoryManager::calculateFreeFrames() const {
    return std::count_if(frames_.begin(), frames_.end(), [](const Frame& f) { return f.free; });
}

std::vector<Frame> MemoryManager::getFrameSnapshot() const {
    return frames_;
}

void MemoryManager::writeToBackingStore(const std::string& filename) const {
    std::ofstream out(filename);
    out << "BaseAddress,Status,Process\n";
    for (const auto& f : frames_) {
        out << f.baseAddr << "," << (f.free ? "FREE" : "USED") << "," << (f.free ? "-" : f.processName) << "\n";
    }
}
