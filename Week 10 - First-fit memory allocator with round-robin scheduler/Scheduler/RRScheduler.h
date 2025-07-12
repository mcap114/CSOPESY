#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <unordered_map>
#include "OsProcess.h"       
#include "BaseScheduler.h"
#include "../Memory/MemoryManager.h"

class RRScheduler : public BaseScheduler {
public:
    RRScheduler(unsigned int numCores = 4, int quantumCycles = 5, int delayPerExec = 10, int maxOverallMem = 16348, int memPerProc = 4096);
    ~RRScheduler();

    RRScheduler(const RRScheduler&) = delete;
    RRScheduler& operator=(const RRScheduler&) = delete;

    void addProcess(std::shared_ptr<OsProcess> process);     
    void shutdown();

    std::shared_ptr<OsProcess> getProcess(const std::string& name) const;  

private:
    void workerLoop(unsigned int coreId);

    const unsigned int numCores;
    const int quantumCycles;
    const int delayPerExec;

    std::atomic<bool> running{ true };

    std::queue<std::shared_ptr<OsProcess>> processQueue;  
    std::unordered_map<std::string, std::shared_ptr<OsProcess>> processMap; 

    mutable std::mutex queueMutex;
    std::condition_variable cv;

    std::vector<std::thread> workerThreads;

    std::unique_ptr<MemoryManager> memoryManager;

    std::atomic<uint32_t> quantumCycleCounter{ 0 };

    void takeMemorySnapshot();
};
