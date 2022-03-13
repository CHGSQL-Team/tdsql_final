#include "descriptor.h"

#include <utility>

WorkDescriptor::WorkDescriptor(std::vector<std::string> _sources, std::string _db_name, std::string _table_name) :
        sources(std::move(_sources)), db_name(std::move(_db_name)), table_name(std::move(_table_name)), table(nullptr) {

}

WorkDescriptor::WorkDescriptor() : table(nullptr) {
}


WorkDescriptor::~WorkDescriptor() {
    delete table;
}

TableDescriptor::~TableDescriptor() {
    for (auto &col: cols) {
        delete col;
    }
}
