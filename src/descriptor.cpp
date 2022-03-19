#include "descriptor.h"

#include <utility>

WorkDescriptor::WorkDescriptor(std::string _db_name, std::string _table_name) :
        db_name(std::move(_db_name)), table_name(std::move(_table_name)), table(nullptr) {

}

WorkDescriptor::~WorkDescriptor() {
}

