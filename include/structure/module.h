#pragma once

#include "structure/descriptor.h"
#include "config.h"
#include "external/sqlconn.h"
#include "utils/logger.h"
#include "utils/timed.h"
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
    boost::asio::thread_pool *pusherPool;

    explicit Module(Config *_config);

    void _init() const;

    ~Module();
};