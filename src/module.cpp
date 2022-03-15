#include "module.h"
#include <boost/filesystem.hpp>


Module::Module(Config *_config) : config(_config) {
    _init();
//    sqlPool = new SQLPool(config);
//    logger = new Logger(config->log_path);
//    subworkerPool = new boost::asio::thread_pool(config->subworker_threads);
//    hasherPool = new boost::asio::thread_pool(config->hasher_threads);
//    reducerPool = new boost::asio::thread_pool(config->reducer_threads);
//    pusherPool = new boost::asio::thread_pool(config->pusher_threads);
}

Module::~Module() {

}

void Module::_init() {
    if (!boost::filesystem::is_directory(config->tmp_path)) {
        boost::filesystem::create_directory(config->tmp_path);
    }
    if (!boost::filesystem::is_directory(config->binlog_path)) {
        boost::filesystem::create_directory(config->binlog_path);
    }
}

