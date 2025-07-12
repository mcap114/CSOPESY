#include "MemoryManager.h"

MemoryManager::MemoryManager(uint32_t totalMemory, uint32_t memPerProc)
    : totalMemory_(totalMemory), memPerProc_(memPerProc) {

    blocks_.push_back({ 0, totalMemory, true, "" }); // entire memory is free initially
}

bool MemoryManager::allocate(const std::string& processName) {
    for (auto& block : blocks_) {
        if (block.free && block.size >= memPerProc_) {
            if (block.size > memPerProc_) {
                // split block
                MemoryBlock newBlock = { block.start + memPerProc_, block.size - memPerProc_, true, "" };
                block.size = memPerProc_;
                blocks_.insert(blocks_.begin() + (&block - &blocks_[0]) + 1, newBlock);
            }
            block.free = false;
            block.processName = processName;
            return true;
        }
    }
    return false; // no sufficient block found
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
