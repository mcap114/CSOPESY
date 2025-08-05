#include "MemoryManager.h"
#include <fstream> 
#include <iostream>
MemoryManager::MemoryManager(uint32_t totalMemory, uint32_t memPerProc)
    : totalMemory_(totalMemory), memPerProc_(memPerProc) {


            std::ofstream log("allocation-debug.log", std::ios::app);
            log << "[MEMMAN] MemoryManager constructor called\n";
            log.close();


    blocks_.push_back({ 0, totalMemory, true, "" }); // entire memory is free initially
}

bool MemoryManager::allocate(const std::string& processName) {
    std::ofstream log("allocation-debug.log", std::ios::app);
    log << "[DEBUG] Trying to allocate for " << processName
        << " | memPerProc_: " << memPerProc_ << "\n";
    log << "[ALLOCATE] MemoryManager address: " << this << "\n";

    for (size_t i = 0; i < blocks_.size(); ++i) {
        log << "[DEBUG] Checking block " << i
            << " | start: " << blocks_[i].start
            << " | size: " << blocks_[i].size
            << " | free: " << blocks_[i].free << "\n";

        if (blocks_[i].free && blocks_[i].size >= memPerProc_) {
            if (blocks_[i].size > memPerProc_) {
                // split current block in place
                MemoryBlock remaining = {
                    blocks_[i].start + memPerProc_,
                    blocks_[i].size - memPerProc_,
                    true,
                    ""
                };

                blocks_[i].size = memPerProc_;
                blocks_.insert(blocks_.begin() + i + 1, remaining);
            }

            // mark block as used
            blocks_[i].free = false;
            blocks_[i].processName = processName;

            log << "[DEBUG] Allocation SUCCESS for " << processName << "\n";
            log << "[DEBUG] Current blocks_ state after allocation:\n";
            for (const auto& blk : blocks_) {
                log << "Start: " << blk.start
                    << ", Size: " << blk.size
                    << ", Free: " << blk.free
                    << ", Process: " << blk.processName << "\n";
            }

            log.close();
            return true;
        }
    }

    log << "[DEBUG] Allocation FAILED for " << processName << "\n";
    log.close();
    return false;
}





void MemoryManager::deallocate(const std::string& processName) {
    for (auto& block : blocks_) {
        if (!block.free && block.processName == processName) {
            block.free = true;
            block.processName = "";

            // merge with next if free
            auto it = &block - &blocks_[0];
            if (it + 1 < blocks_.size() && blocks_[it + 1].free) {
                block.size += blocks_[it + 1].size;
                blocks_.erase(blocks_.begin() + it + 1);
            }

            // merge with prev if free
            if (it > 0 && blocks_[it - 1].free) {
                blocks_[it - 1].size += block.size;
                blocks_.erase(blocks_.begin() + it);
            }

            break;
        }
    }
}

uint32_t MemoryManager::getTotalMemory() {  
    return totalMemory_; 
}


uint32_t MemoryManager::calculateExternalFragmentation() const {
    uint32_t frag = 0;
    for (const auto& block : blocks_) {
        if (block.free && block.size < memPerProc_) {
            frag += block.size;
        }
    }
    return frag;
}

std::vector<MemoryBlock> MemoryManager::getMemorySnapshot() const {
    return blocks_;
}

void MemoryManager::writeToBackingStore(const std::string& filename) const {
    std::ofstream log("allocation-debug.log", std::ios::app);
    log << "[WRITE] MemoryManager address: " << this << "\n";
    log << "[WRITE] Writing blocks_ state:\n";
    for (const auto& blk : blocks_) {
        log << "Start: " << blk.start
            << ", Size: " << blk.size
            << ", Free: " << blk.free
            << ", Process: " << blk.processName << "\n";
    }
    log.close();

    std::ofstream out(filename);
    out << "Start,Size,Status,Process\n";
    for (const auto& b : blocks_) {
        out << b.start << "," << b.size << ","
            << (b.free ? "FREE" : "USED") << ","
            << (b.free ? "-" : b.processName) << "\n";
    }
}