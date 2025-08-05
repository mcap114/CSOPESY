#include "PageTable.h"

PageTable::PageTable(uint32_t pageCount) {
    for (uint32_t i = 0; i < pageCount; ++i) {
        pages_[i] = Page(i);
    }
}

bool PageTable::isPageLoaded(uint32_t vpn) const {
    auto it = pages_.find(vpn);
    return it != pages_.end() && it->second.inMemory;
}

void PageTable::setPageLoaded(uint32_t vpn, uint32_t frameNum) {
    pages_[vpn].inMemory = true;
    pages_[vpn].frameNumber = frameNum;
    pages_[vpn].referenced = true;
    pages_[vpn].dirty = false; // assume clean on load
}

void PageTable::markPageEvicted(uint32_t vpn) {
    pages_[vpn].inMemory = false;
    pages_[vpn].frameNumber = 0;
    pages_[vpn].referenced = false;
}

uint32_t PageTable::getFrameNumber(uint32_t vpn) const {
    return pages_.at(vpn).frameNumber;
}

void PageTable::setFrame(uint32_t vpn, uint32_t frameAddress) {
    pages_[vpn].inMemory = true;
    pages_[vpn].frameNumber = frameAddress;
    pages_[vpn].referenced = true;
    pages_[vpn].dirty = false;
}

const std::unordered_map<uint32_t, Page>& PageTable::getTable() const {
    return pages_;
}