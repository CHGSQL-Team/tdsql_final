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

    void addUniqueIndex(const std::string &name, const std::set<std::string> &colNames, bool isPrimary);

    void dropColumn(const std::string &colName);

    void dropUniqueIndex(bool isPrimary, bool isTemp, const std::string &indexName);

    void changeCol(std::string colName, ColumnDescriptor *newCol);

    void insertRows(std::vector<Row *> &newRows);

    void insertRow(Row *newRow,UniqueIndex* index);

    void print(int trunc);

    size_t getPhyPosArray(int *&array);

    void dumpToFile(const boost::filesystem::path& path);

    void doRowReplace(Row *oldRow, Row *newRow);

    ~Table();

    void optimizeTableForDump();

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

    UniqueIndex(Table *table, std::string name, const std::set<std::string> &colStrs, bool isPrimary);

    void reCompute();

    int checkRow(Row *row);

    void setHashPhy();

    // Get temporary index
    explicit UniqueIndex(Table *table);

    void updateRow(Row *row);

    size_t getHashOfRow(Row *row) const;

    void deleteCol(ColumnDescriptor *delCol);
};

class Row {
public:
    Row(std::vector<std::string> &&data, int source, int stamp);

    std::vector<std::string> data;
    int source;
    int stamp;
    size_t idxInsideTable = -1;
};
