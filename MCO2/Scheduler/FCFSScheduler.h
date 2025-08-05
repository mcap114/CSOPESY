#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <unordered_map>
#include "OSProcess.h"
#include "BaseScheduler.h"

// windows-specific thread initialization
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#endif

class FCFSScheduler : public BaseScheduler {
public:
    FCFSScheduler(unsigned int numCores = 4, int delayPerExec = 10);
    ~FCFSScheduler();

    FCFSScheduler(const FCFSScheduler&) = delete;
    FCFSScheduler& operator=(const FCFSScheduler&) = delete;

    void addProcess(std::shared_ptr<OsProcess> process); // ✅ Updated
    void shutdown();

    std::shared_ptr<OsProcess> getProcess(const std::string& name) const; // ✅ Updated

private:
    void schedule();
    void workerLoop(unsigned int coreId);

    const unsigned int numCores;
    const int delayPerExec;

    std::atomic<bool> running{ true };

    std::queue<std::shared_ptr<OsProcess>> processQueue; // ✅ Updated
    std::vector<std::queue<std::shared_ptr<OsProcess>>> workerQueues; // ✅ Updated

    std::thread schedulerThread;
    std::vector<std::thread> workerThreads;

    mutable std::mutex queueMutex;
    std::condition_variable cv;

    std::unordered_map<std::string, std::shared_ptr<OsProcess>> processMap; // ✅ Updated
};
