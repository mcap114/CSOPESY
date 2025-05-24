// g++ -std=c++17 -o csopesy_cli main.cpp CLI/CLI.cpp
// ./csopesy_cli

#include "CLI/CLI.h"

using namespace std;

int main() {
    
    CLI cli;    // create CLI object
    cli.run();  

    return 0;

}