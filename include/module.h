#pragma once

#include "descriptor.h"
#include "config.h"
#include "sqlconn.h"
#include "logger.h"
#include "timed.h"
#include <map>
#include <vector>
#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/thread_pool.hpp>

class Module {
public:
    Config *config;
    SQLPool *sqlPool;
    Logger *logger;
    Timed timed;
    Works works;
    std::map<unsigned int, TableDescriptor *> tables;

    explicit Module(Config *_config);

    void _init() const;

    ~Module();
};