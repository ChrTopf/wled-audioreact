//
// Created by chrtopf on 04.06.23.
//

#ifndef SRC_CONFIG_H
#define SRC_CONFIG_H
#include <string>
#include <vector>
//3rd party libraries
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Config {
private:
    std::string _filePath;
    json data;
public:
    Config(const std::string &filename);
    std::string getString(const std::string &key, const std::string &defaultVal);
    int getInt(const std::string &key, const int &defaultVal);
    std::vector<std::string> getValues(const std::string &key);
    bool keyExists(const std::string &key);
    bool isEmpty(const std::string &key);
    bool setInt(const std::string &key, const int &value);
    bool setString(const std::string &key, const std::string &value);
    double getDouble(const std::string &key, const double &defaultVal);
    bool setDouble(const std::string &key, const double &value);
};


#endif //SRC_CONFIG_H
