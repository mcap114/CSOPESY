#include "FCFSScheduler.h"
#include <iostream>
#include <thread>

FCFSScheduler::FCFSScheduler(int numCores) : numCores(numCores) {
    workerQueues.resize(numCores);
    schedulerThread = std::thread(&FCFSScheduler::schedule, this);

    for (int i = 0; i < numCores; i++) {
        workerThreads.emplace_back(&FCFSScheduler::workerLoop, this, i);
    }
}

FCFSScheduler::~FCFSScheduler() {
    shutdown();
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(queueMutex);
    processQueue.push(process);
    cv.notify_one();
}

void FCFSScheduler::schedule() {
    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this] { return !processQueue.empty() || !running; });

        if (!running) break;

        for (int core = 0; core < numCores; core++) {
            if (workerQueues[core].empty() && !processQueue.empty()) {
                workerQueues[core].push(processQueue.front());
                processQueue.pop();
            }
        }
        lock.unlock();
    }
}

void FCFSScheduler::workerLoop(int coreId) {
    while (running) {
        std::shared_ptr<Process> process;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (!workerQueues[coreId].empty()) {
                process = workerQueues[coreId].front();
                workerQueues[coreId].pop();
            }
        }

        if (process) {
            while (!process->isCompleted()) {
                process->executePrint(coreId);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::cout << "Process " << process->getName()
                << " completed on core " << coreId << std::endl;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void FCFSScheduler::shutdown() {
    running = false;
    cv.notify_all();

    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }

    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}