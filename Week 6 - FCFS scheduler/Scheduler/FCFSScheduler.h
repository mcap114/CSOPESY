#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "Process.h"

class FCFSScheduler {
public:
    FCFSScheduler(int numCores = 4);
    ~FCFSScheduler();

    void addProcess(std::shared_ptr<Process> process);
    void shutdown();

private:
    void schedule();
    void workerLoop(int coreId);

    int numCores;
    bool running = true;

    std::queue<std::shared_ptr<Process>> processQueue;
    std::vector<std::queue<std::shared_ptr<Process>>> workerQueues;

    std::thread schedulerThread;
    std::vector<std::thread> workerThreads;

    std::mutex queueMutex;
    std::condition_variable cv;
};