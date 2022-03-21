#include "utils/logger.h"
#include "boost/filesystem.hpp"
#include <fstream>
#include <utility>
#include <iostream>


Logger::Logger(std::string _log_path) : log_path(std::move(_log_path)) {
    if (!boost::filesystem::is_directory(log_path)) {
        boost::filesystem::create_directory(log_path);
    }
}

void Logger::write_readable_log(int type, const std::string &key, int content) const {
    size_t isReadyFileName = std::hash<std::string>()(
            std::to_string(log_type::IS_READABLE_LOG_READY) + "::" + std::to_string(type) + "::" + key);
    size_t fileName = std::hash<std::string>()(std::to_string(type) + "::" + key);
    std::ofstream f(log_path + "/" + std::to_string(fileName));
    f << content;
    f.close();
    std::ofstream readable_f(log_path + "/" + std::to_string(isReadyFileName));
    readable_f.close();
}

int Logger::get_readable_log(int type, const std::string &key) const {
    int result;
    size_t isReadyFileName = std::hash<std::string>()(
            std::to_string(log_type::IS_READABLE_LOG_READY) + "::" + std::to_string(type) + "::" + key);
    size_t fileName = std::hash<std::string>()(std::to_string(type) + "::" + key);
    if (!boost::filesystem::is_regular_file(log_path + "/" + std::to_string(isReadyFileName))) return -0x7ffffff;
    std::ifstream f(log_path + "/" + std::to_string(fileName));
    f >> result;
    return result;

}

bool Logger::find_log(int type, const std::string &key) const {
    size_t file_name = std::hash<std::string>()(std::to_string(type) + "::" + key);
    return boost::filesystem::is_regular_file(log_path + "/" + std::to_string(file_name));
}

void Logger::write_normal_log(int type, const std::string &key) const {
    size_t file_name = std::hash<std::string>()(std::to_string(type) + "::" + key);
    std::ofstream f(log_path + "/" + std::to_string(file_name));
    f.close();
}
