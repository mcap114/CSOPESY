#include "CLI/CLI.h"
#include <iostream>

int main() {
    Config config;
    config.load("config.txt");

	//debug output to verify configuration loading
    std::cout << "Loaded configuration:\n";
    for (const auto& key : config.getOrder()) {
        std::cout << key << " = " << config.getString(key) << "\n";
    }

    try {
        CLI cli(config);
        cli.run();

    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing CLI: " << e.what() << "\n";
        return 1;
    }

    

    return 0;
}