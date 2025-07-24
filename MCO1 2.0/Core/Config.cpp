#include "Config.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (iss >> key) {
            std::string value;
            std::getline(iss, value);
            // Remove leading whitespace
            value.erase(0, value.find_first_not_of(" \t"));

            // Remove surrounding quotes if any
            if (!value.empty() && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }

            if (parameters.find(key) == parameters.end()) {
                order.push_back(key);
            }

            parameters[key] = value;
        }
    }
}


int Config::getInt(const std::string& key) const {
    return std::stoi(parameters.at(key));
}

std::string Config::getString(const std::string& key) const {
    return parameters.at(key);
}