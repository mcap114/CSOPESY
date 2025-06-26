#pragma once
#include <memory>
#include <string>
#include "Process.h"

class BaseScheduler {
public:
    virtual ~BaseScheduler() = default;

    virtual void addProcess(std::shared_ptr<Process> process) = 0;
    virtual void shutdown() = 0;
    virtual std::shared_ptr<Process> getProcess(const std::string& name) const = 0;
};
