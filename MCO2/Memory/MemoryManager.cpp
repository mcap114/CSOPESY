#include "MemoryManager.h"

MemoryManager::MemoryManager(uint32_t totalMemory, uint32_t memPerProc)
    : totalMemory_(totalMemory), memPerProc_(memPerProc) {

    blocks_.push_back({ 0, totalMemory, true, "" }); // entire memory is free initially
}

bool MemoryManager::allocate(const std::string& processName) {
    for (size_t i = 0; i < blocks_.size(); ++i) {
        if (blocks_[i].free && blocks_[i].size >= memPerProc_) {
            if (blocks_[i].size > memPerProc_) {
                MemoryBlock newBlock = { blocks_[i].start + memPerProc_, blocks_[i].size - memPerProc_, true, "" };
                blocks_[i].size = memPerProc_;
                blocks_.insert(blocks_.begin() + i + 1, newBlock);
            }
            blocks_[i].free = false;
            blocks_[i].processName = processName;
            return true;
        }
    }
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
