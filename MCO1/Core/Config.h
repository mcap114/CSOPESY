#pragma once
#include <string>
#include <unordered_map>

class Config {
public:
    void load(const std::string& filename);
    int getInt(const std::string& key) const;
    std::string getString(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> parameters;
};