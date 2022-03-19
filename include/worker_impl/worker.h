#pragma once
#include "module.h"

class Worker {
private:
    Module* module;
public:
    explicit Worker(Module* module);

    void work();
};