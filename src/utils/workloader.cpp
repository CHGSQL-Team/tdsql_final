#include "utils/workloader.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <utility>
#include <boost/range.hpp>

WorkLoader::WorkLoader(Module *_module) : module(_module) {

}

void WorkLoader::loadWorks() const {
    boost::filesystem::path data_path(module->config->binlog_path);
    for (auto &entry: boost::make_iterator_range(boost::filesystem::directory_iterator(data_path), {}))
        if (boost::filesystem::is_directory(entry))
            _loadTable(entry.path());
}

void WorkLoader::_loadTable(const boost::filesystem::path &dbPath) const {

    for (auto &entry: boost::make_iterator_range(boost::filesystem::directory_iterator(dbPath), {})) {
        if (boost::filesystem::is_directory(entry)) {
            std::string dbName = entry.path().parent_path().stem().generic_string();
            std::string tableName = entry.path().stem().generic_string();
            boost::filesystem::path stateCountPath = entry.path();
            stateCountPath.append("/0/cnt.txt");
            std::ifstream stateCountReader(stateCountPath.c_str());
            int stateCount;
            stateCountReader >> stateCount;
            module->works.data[std::make_pair(dbName, tableName)] = new WorkDescriptor(dbName, tableName, entry,
                                                                                       stateCount + 1);
        }
    }
}

void WorkLoader::printWorks() const {
    for (const auto &i: module->works) {
        std::cout << "[WL Elem] DB: " << i.first.first << " Table: " << i.first.second << std::endl;
    }
}