#include <string> // Include the string header for std::string  
#include <unordered_map>  

class Config {  
public:  
    void load(const std::string& filename);  
    int getInt(const std::string& key) const;  
    std::string getString(const std::string& key) const;  

 

    const std::unordered_map<std::string, std::string>& getAll() const {
        return parameters;
    }


private:  
    std::unordered_map<std::string, std::string> parameters;  
};