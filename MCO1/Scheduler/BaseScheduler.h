#pragma once
#include <memory>
#include <vector>
#include "Process.h"

class BaseScheduler {
public:
    virtual ~BaseScheduler() = default;
    virtual void addProcess(std::shared_ptr<Process> process) = 0;
    virtual void shutdown() = 0;
    virtual void start() = 0;

    // Common functionality
    unsigned int getNumCores() const { return numCores; }

protected:
    explicit BaseScheduler(unsigned int numCores) : numCores(numCores) {}
    unsigned int numCores;
};