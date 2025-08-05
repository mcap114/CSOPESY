#pragma once
#include "Page.h"
#include <unordered_map>
#include <cstdint>

class PageTable {
public:
    PageTable(uint32_t pageCount);

    bool isPageLoaded(uint32_t virtualPageNumber) const;
    void setPageLoaded(uint32_t virtualPageNumber, uint32_t frameNumber);
    void markPageEvicted(uint32_t virtualPageNumber);
    uint32_t getFrameNumber(uint32_t virtualPageNumber) const;

    void setFrame(uint32_t virtualPageNumber, uint32_t frameAddress);
    const std::unordered_map<uint32_t, Page>& getTable() const;
private:
    std::unordered_map<uint32_t, Page> pages_;
};