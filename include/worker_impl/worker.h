#pragma once
#include "structure/module.h"

class Worker {
private:
    Module* module;
public:
    explicit Worker(Module* module);

    void work();

    ~Worker();

    void i_am_not_willing_to_pass();
};