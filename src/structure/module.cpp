#include "structure/module.h"
#include <boost/filesystem.hpp>


Module::Module(Config *_config) : config(_config) {
    _init();
    logger = new Logger(config->log_path);
    sqlPool = new SQLPool(config);
    pusherPool = new boost::asio::thread_pool(config->pusher_threads);
    subworkerPool = new boost::asio::thread_pool(config->subworker_threads);
}

Module::~Module() {
    delete sqlPool;
    delete pusherPool;
    delete subworkerPool;
}

void Module::_init() const {
    if (!boost::filesystem::is_directory(config->tmp_path)) {
        boost::filesystem::create_directory(config->tmp_path);
    }
    if (!boost::filesystem::is_directory(config->binlog_path)) {
        boost::filesystem::create_directory(config->binlog_path);
    }
    if (!boost::filesystem::is_directory(config->log_path)) {
        boost::filesystem::create_directory(config->log_path);
    }
}

