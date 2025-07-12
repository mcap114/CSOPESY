#include "RRScheduler.h"
#include <chrono>
#include <iostream>

// windows-specific thread initialization 
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#endif

RRScheduler::RRScheduler(unsigned int numCores, int quantumCycles, int delayPerExec, int maxOverallMem, int memPerProc)
    : numCores(numCores), quantumCycles(quantumCycles), delayPerExec(delayPerExec) {

    memoryManager = std::make_unique<MemoryManager>(maxOverallMem, memPerProc);

    for (unsigned int i = 0; i < numCores; ++i) {
        workerThreads.emplace_back([this, i] { this->workerLoop(i); });
    }

    // std::cout << "RR Scheduler initialized with " << numCores << " cores, quantum " << quantumCycles << ", delay " << delayPerExec << "ms\n";
}

RRScheduler::~RRScheduler() {
    shutdown();
}

void RRScheduler::addProcess(std::shared_ptr<OsProcess> process) {  
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        processQueue.push(process);
        processMap[process->getName()] = process;
    }
    cv.notify_all();
}

std::shared_ptr<OsProcess> RRScheduler::getProcess(const std::string& name) const { 
    std::lock_guard<std::mutex> lock(queueMutex);
    auto it = processMap.find(name);
    if (it != processMap.end()) {
        return it->second;
    }
    return nullptr;
}

void RRScheduler::workerLoop(unsigned int coreId) {
    while (running) {
        std::shared_ptr<OsProcess> process = nullptr;  

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this] {
                return !processQueue.empty() || !running;
                });

            if (!running) return;

            if (!processQueue.empty()) {
                process = processQueue.front();
                processQueue.pop();
            }
        }

        if (process) {
            int executed = 0;

            while (!process->isCompleted() && executed < quantumCycles && running) {
                process->executeNextInstruction(coreId);
                std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec));
                executed++;
            }

            if (!process->isCompleted() && running) {
                std::lock_guard<std::mutex> lock(queueMutex);
                processQueue.push(process); // put it back at the end
                cv.notify_all();
            }
        }
    }
}

void RRScheduler::shutdown() {
    running = false;
    cv.notify_all();

    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "Round-Robin Scheduler shutdown complete\n";
}
