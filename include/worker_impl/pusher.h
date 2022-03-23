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

    void createFinalTable();
    void pushFromFile();

    void flushSQL(const std::string& sqlHeader,const std::string& sqlContent);
};