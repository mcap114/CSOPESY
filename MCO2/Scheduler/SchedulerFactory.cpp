#include "SchedulerFactory.h"  
#include "FCFSScheduler.h"  
#include "RRScheduler.h"  
#include <stdexcept>  

std::unique_ptr<Scheduler> createSchedulerFromConfig(const Config& config, ScreenManager& sm) {  
    std::string sched = config.getString("scheduler");  
    if (sched == "fcfs") {  
        return std::make_unique<FCFScheduler>(sm);  
    }  
    else if (sched == "rr") {  
        int quantum = config.getInt("quantum-cycles");  
        return std::make_unique<RRScheduler>(sm, quantum);  
    }  
    else {  
        throw std::runtime_error("Invalid scheduler type: " + sched);  
    }  
}
