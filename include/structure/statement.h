#pragma once

#include "boost/filesystem.hpp"
#include "table.h"

#include <string>
#include <vector>
#include <set>

enum class AlterType {
    NOTHING,
    ADDCOL,
    DROPCOL,
    CHANGECOL,
    ADDINDEX,
    DROPINDEX,
};

class Statement {
public:
};

class ColumnStatement;

class IndexStatement;

class CreateTableStatement : public Statement {
public:
    explicit CreateTableStatement(const boost::filesystem::path &path,
                                  const boost::filesystem::path &sqlpath);

    std::string name;
    std::vector<ColumnStatement> cols;
    std::vector<IndexStatement> indexs;

    void print();

    void fillToTable(Table *table);
};

class ColumnStatement {
public:
    std::string name;
    bool isNotNull;
    std::string *defaultStr;

    void print() const;
};

class IndexStatement {
public:
    std::string name;
    bool isPrimaryKey;
    std::set<std::string> keys;
};

class AlterStatement : public Statement {
public:
    AlterType type;

    explicit AlterStatement(AlterType type);

    static AlterStatement *getAlterStatement(const boost::filesystem::path &path,
                                             const boost::filesystem::path &sqlpath);

    virtual void print() = 0;
};

class AlterAddColStatement : public AlterStatement {
public:
    AlterAddColStatement(ColumnStatement colStat, std::string *insAfter);

    ColumnStatement colStat;
    std::string *insAfter;

    void print() override;
};
