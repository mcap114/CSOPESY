#pragma once
#include <memory>
#include <string>
#include "OSProcess.h"

class BaseScheduler {
public:
    virtual ~BaseScheduler() = default;

    virtual void addProcess(std::shared_ptr<OsProcess> process) = 0;
    virtual void shutdown() = 0;
    virtual std::shared_ptr<OsProcess> getProcess(const std::string& name) const = 0;
};
