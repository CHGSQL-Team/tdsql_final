#include "descriptor.h"

#include <utility>

WorkDescriptor::WorkDescriptor(std::string _db_name, std::string _table_name, boost::filesystem::path binlogPath,
                               int stateCount) :
        db_name(std::move(_db_name)), table_name(std::move(_table_name)), binlogPath(std::move(binlogPath)),
        table(nullptr), stateCount(stateCount) {
}

WorkDescriptor::~WorkDescriptor() {
}

