#pragma once

#include <string>
#include <mutex>

enum log_type {
    TABLE_CREATED,
    LINE_COUNT,
    IS_READABLE_LOG_READY,
    FINISHED,
    HSHTMP_MAX,
    DAT_AVAL_COUNT,
    PRIMARY_KEY,
};


class Logger {
public:
    std::string log_path;

    std::mutex mtx;

    explicit Logger(std::string _log_path);

    void write_normal_log(int type, const std::string &key) const;

    void write_readable_log(int type, const std::string &key, int content) const;

    int get_readable_log(int type, const std::string &key) const;

    bool find_log(int type, const std::string &key) const;

};