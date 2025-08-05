#pragma once
#include <cstdint>

struct Page {
    uint32_t virtualPageNumber;
    uint32_t frameNumber;
    bool inMemory;
    bool dirty;
    bool referenced;

    Page(uint32_t vpn = 0)
        : virtualPageNumber(vpn), frameNumber(0), inMemory(false), dirty(false), referenced(false) {
    }
};