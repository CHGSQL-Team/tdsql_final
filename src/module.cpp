#include "module.h"
#include <boost/filesystem.hpp>


Module::Module(Config *_config) : config(_config) {
    _init();
    logger = new Logger(config->log_path);
    sqlPool = new SQLPool(config);
}

Module::~Module() {

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

