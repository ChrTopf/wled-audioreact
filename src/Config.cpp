//
// Created by chrtopf on 04.06.23.
//

#include "Config.h"
#include <fstream>
#include <sstream>
#include "Log.h"

Config::Config(const std::string &filename) {
    _filePath = filename;
    //try to open the file
    std::ifstream inputStream;
    inputStream.open(_filePath);
    if(inputStream){
        //check if the file is empty
        if(inputStream.peek() == std::ifstream::traits_type::eof()){
            //report that the file is empty
            std::stringstream ss;
            ss << "The config file '" << _filePath << "' is empty. Please copy the default one!";
            Log::e(ss.str());
            //exit the application
            std::exit(1);
        }
        //if everything seems to be alright, safe the filename
        data = json::parse(inputStream);
        //do not forget to close the input stream
        inputStream.close();
    }else{
        //report that the file could not be opened
        std::stringstream ss;
        ss << "Could not open config file '" << _filePath << "'. Are the right permissions set? Is the file present?";
        Log::e(ss.str());
        //exit the application
        std::exit(1);
    }
}

std::string Config::getString(const std::string &key, const std::string &defaultVal) {
    // Access the values existing in JSON data
    return data.value(key, defaultVal);
}

int Config::getInt(const std::string &key, const int &defaultVal) {
    return data.value(key, defaultVal);
}

std::vector<std::string> Config::getValues(const std::string &key) {
    //create an array
    std::vector<std::string> result;
    //read each value
    for (auto& el : data.at(key).items()){
        result.push_back(el.value());
    }
    //return the result as an array
    return result;
}

bool Config::keyExists(const std::string &key) {
    return data.contains(key);
}

bool Config::isEmpty(const std::string &key) {
    return data.at(key).empty();
}

void Config::setInt(const std::string &key, const int &value) {
    data[key] = value;
}

void Config::setString(const std::string &key, const std::string &value) {
    data[key] = value;
}

double Config::getDouble(const std::string &key, const double &defaultVal) {
    return data.value(key, defaultVal);
}

void Config::setDouble(const std::string &key, const double &value) {
    data[key] = value;
}

bool Config::saveConfig() {
    std::ofstream file(_filePath);
    file << data;
    return true;
}

