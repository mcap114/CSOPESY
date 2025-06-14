#pragma once
#include <unordered_map>
#include <string>
#include "Process.h"

class ProcessRegistry {
public:

    void addProcess(Process p) {
        std::string name = p.getName(); 
        registry_[name] = p; 
    }

    const Process* findProcess(const std::string& name) const {
        if (registry_.count(name) > 0) {
            return &registry_.at(name); 
        }
    }

    bool doesProcessExist(const std::string& name) const {
        return registry_.count(name) > 0;
    }

private:
    std::unordered_map<std::string, Process> registry_; 
};

