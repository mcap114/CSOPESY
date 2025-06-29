#include "FCFSScheduler.h"
#include <iostream>
#include <chrono>

FCFSScheduler::FCFSScheduler(unsigned int numCores, int delayPerExec)
    : numCores(numCores), delayPerExec(delayPerExec),
    workerQueues(numCores) {

    // lambdas to avoid direct member function pointer issues
    schedulerThread = std::thread([this] { this->schedule(); });

    for (unsigned int i = 0; i < numCores; ++i) {
        workerThreads.emplace_back([this, i] { this->workerLoop(i); });
    }

    //std::cout << "FCFS Scheduler initialized with " << numCores << " cores\n";
}

FCFSScheduler::~FCFSScheduler() {
    shutdown();
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        processQueue.push(process);
        processMap[process->getName()] = process; 
    }
    cv.notify_one();
}

std::shared_ptr<Process> FCFSScheduler::getProcess(const std::string& name) const {
    std::lock_guard<std::mutex> lock(queueMutex);
    auto it = processMap.find(name);
    if (it != processMap.end()) {
        return it->second;
    }
    return nullptr;
}

void FCFSScheduler::schedule() {
    unsigned int nextCore = 0;

    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this] {
            return !processQueue.empty() || !running;
        });

        if (!running) break;

        // Assign processes one by one to cores in round-robin
        while (!processQueue.empty()) {
            workerQueues[nextCore].push(processQueue.front());
            processQueue.pop();

            // Move to next core (cycle through cores)
            nextCore = (nextCore + 1) % numCores;
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
                process->executeNextInstruction(coreId);
                std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec));
            }

            // uncomment for real-time status updating, otherwise this gets in the way of screen -ls command
            if (running) {
                //std::cout << "Process " << process->getName()
                //    << " completed on core " << coreId << "\n";

            }
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

    std::cout << "Scheduler shutdown complete\n";
}