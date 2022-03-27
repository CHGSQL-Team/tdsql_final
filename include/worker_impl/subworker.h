#pragma once

#include "structure/module.h"
#include "structure/statement.h"
#include <string>

class SubWorker {
private:
    Module *module;
    WorkDescriptor *workDes;
public:
    explicit SubWorker(Module *module, WorkDescriptor *workDes);

    void work();

    void Trace(Statement* statement_, int state);

    ~SubWorker();
};