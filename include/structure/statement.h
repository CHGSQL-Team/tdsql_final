#pragma once

#include "boost/filesystem.hpp"
#include "utils/iohelper.h"
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
    static Statement *getStatement(const boost::filesystem::path &path,
                                   const boost::filesystem::path &sqlpath);

    virtual void print() = 0;

    virtual void fillToTable(Table *table) = 0;
};

class ColumnStatement;

class IndexStatement;

class CreateTableStatement : public Statement {
public:
    static CreateTableStatement *getCreateTableStatement(IOHelper &iohelper);

    explicit CreateTableStatement(IOHelper &ioHelper);

    std::string name;
    std::vector<ColumnStatement> cols;
    std::vector<IndexStatement> indexs;

    void print() override;

    void fillToTable(Table *table) override;
};

class ColumnStatement {
public:
    std::string name;
    bool isNotNull;
    std::string *defaultStr = nullptr;

    void print() const;

    explicit ColumnStatement(IOHelper &helper);
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

    static AlterStatement *getAlterStatement(IOHelper &ioHelper);

    void print() override = 0;

    void fillToTable(Table *table) override = 0;
};

class AlterAddColStatement : public AlterStatement {
public:
    // Deprecated!
    AlterAddColStatement(ColumnStatement colStat, std::string *insAfter);

    explicit AlterAddColStatement(IOHelper &ioHelper);

    ColumnStatement colStat;
    std::string *insAfter;

    void print() override;

    void fillToTable(Table *table) override;
};

class AlterNothingStatement : public AlterStatement {
public:
    AlterNothingStatement();

    void print() override;

    void fillToTable(Table *table) override;
};

class AlterDropColStatement : public AlterStatement {
public:
    std::string colName;

    // Deprecated!
    explicit AlterDropColStatement(std::string colName);

    explicit AlterDropColStatement(IOHelper &ioHelper);

    void print() override;

    void fillToTable(Table *table) override;
};

class AlterDropIndexStatement : public AlterStatement {
public:
    bool isPrimary;
    std::string indexName;

    void print() override;

    void fillToTable(Table *table) override;

    explicit AlterDropIndexStatement(IOHelper &ioHelper);
};

class AlterAddIndexStatement : public AlterStatement {
public:
    bool isPrimary;
    std::string indexName;
    std::set<std::string> cols;

    void print() override;

    void fillToTable(Table *table) override;

    explicit AlterAddIndexStatement(IOHelper &ioHelper);
};

class AlterChangeColStatement : public AlterStatement {
public:
    std::string prevName;

    explicit AlterChangeColStatement(IOHelper &ioHelper);

    ColumnStatement colStat;

    void print() override;

    void fillToTable(Table *table) override;
};