#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <unordered_map>
#include "Process.h"
#include "BaseScheduler.h"

// windows-specific thread initialization 
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#endif

class RRScheduler : public BaseScheduler {
public:
    RRScheduler(unsigned int numCores = 4, int quantumCycles = 5, int delayPerExec = 10);
    ~RRScheduler();

    RRScheduler(const RRScheduler&) = delete;
    RRScheduler& operator=(const RRScheduler&) = delete;

    void addProcess(std::shared_ptr<Process> process);
    void shutdown();

    std::shared_ptr<Process> getProcess(const std::string& name) const;

private:
    void workerLoop(unsigned int coreId);

    const unsigned int numCores;
    const int quantumCycles;
    const int delayPerExec;

    std::atomic<bool> running{ true };

    std::queue<std::shared_ptr<Process>> processQueue;
    std::unordered_map<std::string, std::shared_ptr<Process>> processMap;

    mutable std::mutex queueMutex;
    std::condition_variable cv;

    std::vector<std::thread> workerThreads;
};
