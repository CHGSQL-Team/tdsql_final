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

    explicit Table(WorkDescriptor *workDes);

    void addColumn(ColumnDescriptor *newCol, std::string *after);

    void addUniqueIndex(const std::string& name, const std::set<std::string>& colNames, bool isPrimary);

    void dropColumn(std::string colName);

    void dropUniqueIndex(std::string indexName);

    void changeCol(std::string colName, ColumnDescriptor *newCol);

    void insertRows(std::vector<Row *> &newRows);

    void insertRow(Row *newRow);

    void print(int trunc);

    void getInsPhyArray(int *array);
};

class UniqueIndex {
public:
    Table *table;
    std::string name;
    std::set<ColumnDescriptor *> cols;
    std::unordered_map<size_t, Row *> hash;
    bool isPrimary;
    bool isTemp;
    int *hashPhy = nullptr;

    UniqueIndex(Table *table, std::string name, const std::set<std::string>& colStrs, bool isPrimary);

    void reCompute();

    int checkRow(Row *row);

    // Get temporary index
    explicit UniqueIndex(Table *table);
};

class Row {
public:
    Row(std::vector<std::string> &&data, int source, int stamp);

    std::vector<std::string> data;
    int source;
    int stamp;
};
