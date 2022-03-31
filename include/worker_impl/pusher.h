#pragma once
#include "structure/descriptor.h"
#include "structure/module.h"

class Pusher{
private:

public:
    std::string dbName,tableName;
    boost::filesystem::path binlogPath;
    int stateCount;
    Module* module;


    explicit Pusher(WorkDescriptor *workDes, Module *module);

    void push() const;

    void createFinalTable() const;
    void pushFromFile() const;

    static void flushSQL(const std::string &sqlHeader, const std::string &sqlContent, SQLInstance *instance) ;
};