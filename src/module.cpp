#include "module.h"
#include "boost/filesystem.hpp"

Module::Module(Config *_config) : config(_config) {
//    sqlPool = new SQLPool(config);
//    logger = new Logger(config->log_path);
//    subworkerPool = new boost::asio::thread_pool(config->subworker_threads);
//    hasherPool = new boost::asio::thread_pool(config->hasher_threads);
//    reducerPool = new boost::asio::thread_pool(config->reducer_threads);
//    pusherPool = new boost::asio::thread_pool(config->pusher_threads);
}

Module::~Module() {

}

