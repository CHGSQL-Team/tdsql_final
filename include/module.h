#pragma once

#include "descriptor.h"
#include "config.h"
#include "sqlconn.h"
#include "logger.h"
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
    boost::asio::thread_pool *subworkerPool;
    boost::asio::thread_pool *hasherPool;
    boost::asio::thread_pool *reducerPool;
    boost::asio::thread_pool *pusherPool;
    Works works;
    std::map<unsigned int, TableDescriptor *> tables;

    explicit Module(Config *_config);

    ~Module();
};