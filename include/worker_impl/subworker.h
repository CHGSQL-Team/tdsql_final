#pragma once

#include "structure/module.h"
#include "structure/statement.h"
#include <string>

class SubWorker {
private:
    Module *module;
    std::string dbName, tableName;
    int traceIndex;
    WorkDescriptor *workDes;
public:
    explicit SubWorker(Module *module, WorkDescriptor *workDes);

    void work();

    void Trace(Statement* statement_, int state);

    ~SubWorker();
};