#pragma once
#include "structure/module.h"

class Worker {
private:
    Module* module;
public:
    explicit Worker(Module* module);

    void work();
};