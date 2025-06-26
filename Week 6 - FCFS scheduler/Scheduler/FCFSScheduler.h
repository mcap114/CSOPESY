#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include "Process.h"

// windows-specific thread initialization 
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#endif

class FCFSScheduler {
public:
    explicit FCFSScheduler(unsigned int numCores = 4); // declare number of CPU cores here
    ~FCFSScheduler();

    // disable copying
    FCFSScheduler(const FCFSScheduler&) = delete;
    FCFSScheduler& operator=(const FCFSScheduler&) = delete;

    void addProcess(std::shared_ptr<Process> process);
    unsigned int getNumCores();
    void shutdown();

private:
    void schedule();
    void workerLoop(unsigned int coreId);


    const unsigned int numCores;
    std::atomic<bool> running{ true };

    std::queue<std::shared_ptr<Process>> processQueue;
    std::vector<std::queue<std::shared_ptr<Process>>> workerQueues;

    std::thread schedulerThread;
    std::vector<std::thread> workerThreads;

    mutable std::mutex queueMutex;
    std::condition_variable cv;
};