#pragma once
#include <vector>

class ReportGenerator {
public:
    ReportGenerator(int numCores);

    void recordExecution(int coreId);
    void recordCompletion();
    void setTotalTicks(unsigned long long ticks);

    void printReport() const;

private:
    int numCompleted = 0;
    std::vector<unsigned long long> coreUsage;
    unsigned long long totalTicks = 0;
};
