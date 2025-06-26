#include "FCFSScheduler.h"
#include <iostream>
#include <chrono>
#include "../Core/CPUTimer.h"

FCFSScheduler::FCFSScheduler(unsigned int numCores)
    : numCores(numCores),
      workerQueues(numCores) {

    schedulerThread = std::thread([this] { this->schedule(); });

    for (unsigned int i = 0; i < numCores; ++i) {
        workerThreads.emplace_back([this, i] { this->workerLoop(i); });
    }

    std::cout << "FCFS Scheduler initialized with " << numCores << " cores\n";
}

FCFSScheduler::~FCFSScheduler() {
    shutdown();
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        processQueue.push(process);
    }
    cv.notify_one();
}

void FCFSScheduler::schedule() {
    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this] {
            return !processQueue.empty() || !running;
        });

        if (!running) break;

        for (unsigned int core = 0; core < numCores; ++core) {
            if (workerQueues[core].empty() && !processQueue.empty()) {
                workerQueues[core].push(processQueue.front());
                processQueue.pop();
            }
        }
    }
}

void FCFSScheduler::workerLoop(unsigned int coreId) {
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
            while (!process->isCompleted() && running) {
                CPUTimer::tick();
                process->step();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            // optional: print when done
            // std::cout << "Process " << process->getName()
            //           << " completed on core " << coreId << "\n";
        } else {
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

    std::cout << "Scheduler shutdown complete\n";
}

// ✅ Implementation of findProcess()
std::shared_ptr<Process> FCFSScheduler::findProcess(const std::string& name) {
    std::lock_guard<std::mutex> lock(queueMutex);

    // Search in global queue
    std::queue<std::shared_ptr<Process>> tempQueue = processQueue;
    while (!tempQueue.empty()) {
        auto p = tempQueue.front();
        tempQueue.pop();
        if (p->getName() == name) return p;
    }

    // Search in all worker queues
    for (const auto& q : workerQueues) {
        std::queue<std::shared_ptr<Process>> temp = q;
        while (!temp.empty()) {
            auto p = temp.front();
            temp.pop();
            if (p->getName() == name) return p;
        }
    }
    return nullptr;
}
