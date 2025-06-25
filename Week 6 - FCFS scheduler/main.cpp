// g++ -std=c++17 -ICLI -IScheduler -IScreen CLI/CLI.cpp Scheduler/FCFSScheduler.cpp Scheduler/Process.cpp Screen/Screen.cpp Screen/ScreenManager.cpp main.cpp -o main
// ./main

#include "CLI/CLI.h"

using namespace std;

int main() {
    
    CLI cli;    // create CLI object
    cli.run();  

    return 0;

}
