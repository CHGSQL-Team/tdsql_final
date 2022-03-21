#pragma once

#include <string>
#include "descriptor.h"
#include <unordered_map>
#include <set>

class ColumnDescriptor;

class Row;

class WorkDescriptor;

class UniqueIndex;

class Table {
public:
    WorkDescriptor *workDes;
    int colPhy = 0;
    std::vector<ColumnDescriptor *> colDes;
    ColumnDescriptor *updCol = nullptr;
    std::unordered_map<std::string, ColumnDescriptor *> nameToColDes;
    std::vector<Row *> rows;
    std::vector<UniqueIndex *> indexs;
    UniqueIndex *priIndex = nullptr;

    explicit Table(WorkDescriptor *workDes);

    void addColumn(ColumnDescriptor *newCol, std::string *after);

    void addUniqueIndex(std::string name, std::set<std::string> colNames);

    void dropColumn(std::string colName);

    void dropUniqueIndex(std::string indexName);

    void changeCol(std::string colName, ColumnDescriptor *newCol);

    void print(int trunc);
};

class UniqueIndex {
public:
    Table *table;
    std::string name;
    std::set<ColumnDescriptor *> cols;
    std::unordered_map<size_t, Row *> hash;

    void reCompute();
};

class Row{
public:
    std::vector<std::string> data;
    int source;
    time_t timestamp;
};
