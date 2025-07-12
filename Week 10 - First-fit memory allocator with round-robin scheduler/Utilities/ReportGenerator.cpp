#include "ReportGenerator.h"
#include <iostream>

ReportGenerator::ReportGenerator(int numCores)
    : coreUsage(numCores, 0) {
}

void ReportGenerator::recordExecution(int coreId) {
    if (coreId >= 0 && coreId < coreUsage.size()) {
        ++coreUsage[coreId];
    }
}

void ReportGenerator::recordCompletion() {
    ++numCompleted;
}

void ReportGenerator::setTotalTicks(unsigned long long ticks) {
    totalTicks = ticks;
}

void ReportGenerator::printReport() const {
    std::cout << "\n--- CPU Utilization Report ---\n";
    std::cout << "Total CPU Ticks: " << totalTicks << "\n";
    std::cout << "Processes Completed: " << numCompleted << "\n";

    for (size_t i = 0; i < coreUsage.size(); ++i) {
        double utilization = totalTicks > 0 ?
            (100.0 * coreUsage[i] / totalTicks) : 0.0;
        std::cout << "Core " << i << ": "
            << coreUsage[i] << " active ticks ("
            << utilization << "% utilization)\n";
    }
}
