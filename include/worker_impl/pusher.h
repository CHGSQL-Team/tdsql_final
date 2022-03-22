#pragma once
#include "structure/descriptor.h"
#include "structure/module.h"

class Pusher{
private:
    WorkDescriptor* workDes;
    Module* module;
public:
    explicit Pusher(WorkDescriptor *workDes, Module *module);

    void push();
};