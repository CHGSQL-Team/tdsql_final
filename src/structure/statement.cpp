#include "structure/statement.h"
#include "utils/iohelper.h"
#include <fstream>
#include <iostream>
#include <utility>
#include "utils/uniformlog.h"

CreateTableStatement::CreateTableStatement(IOHelper &helper) {
    std::string tableName;
    tableName = helper.getLine();
    int colCount = helper.getLineInt();
    name = tableName;
    for (int i = 0; i < colCount; i++) {
        std::string colName = helper.getLine();
        std::string defValue;
        int isNotNull = helper.getLineInt();
        int isDef = helper.getLineInt();
        if (isDef) defValue = helper.getLine();
        cols.push_back({colName, static_cast<bool>(isNotNull), isDef ? new std::string(defValue) : nullptr});
    }
    int idxCount = helper.getLineInt();
    for (int i = 0; i < idxCount; i++) {
        std::string idxName;
        int hasName = helper.getLineInt();
        if (hasName) idxName = helper.getLine();
        int isPrimary = helper.getLineInt();
        int keyCount = helper.getLineInt();
        std::set<std::string> keys;
        for (int j = 0; j < keyCount; j++) {
            std::string keyName = helper.getLine();
            keys.insert(keyName);
        }
        indexs.push_back({idxName, static_cast<bool>(isPrimary), std::move(keys)});
    }
}

void CreateTableStatement::print() {
    UniformLog log("Create Table Statement", name);
    for (const auto &col: cols) {
        std::cout << "Col: " << col.name << (col.isNotNull ? " NOT NULL " : " CAN NULL ");
        if (col.defaultStr) std::cout << " Def: " << *col.defaultStr;
        std::cout << std::endl;
    }
    for (const auto &idx: indexs) {
        std::cout << "Idx: \"" << idx.name << "\"";
        if (idx.isPrimaryKey) std::cout << " PRI ";
        std::cout << " Keys: ";
        for (const auto &key: idx.keys) {
            std::cout << key << ",";
        }
        std::cout << std::endl;
    }
}

void CreateTableStatement::fillToTable(Table *table) {
    for (const auto &col: cols) {
        auto *newColDes = new ColumnDescriptor(col.name, col.defaultStr);
        table->addColumn(newColDes, nullptr);
    }
    for (const auto &index: indexs) {
        table->indexs.push_back(new UniqueIndex(table, index.name, index.keys, index.isPrimaryKey));
    }
    if (indexs.empty()) {
        table->indexs.push_back(new UniqueIndex(table));
    }
}

CreateTableStatement *CreateTableStatement::getCreateTableStatement(IOHelper &iohelper) {
    return new CreateTableStatement(iohelper);
}


AlterStatement *AlterStatement::getAlterStatement(IOHelper &helper) {
    std::string typeStr = helper.getLine();
    if (typeStr == "ADDCOL") {
        std::string colName = helper.getLine();
        int isNotNull = helper.getLineInt();
        int isDef = helper.getLineInt();
        std::string *def = nullptr;
        if (isDef)
            def = new std::string(helper.getLine());
        int isAfterCol = helper.getLineInt();
        std::string *after = nullptr;
        if (isAfterCol)
            after = new std::string(helper.getLine());
        ColumnStatement colStat{colName, static_cast<bool>(isNotNull), def};
        return new AlterAddColStatement(colStat, after);
    } else if (typeStr == "NOTHING") {
        return new AlterNothingStatement();
    }
    throw std::runtime_error("Alter type no implemented!");
    return nullptr;
}

AlterStatement::AlterStatement(AlterType type) : type(type) {

}

AlterAddColStatement::AlterAddColStatement(ColumnStatement colStat, std::string *insAfter)
        : AlterStatement(AlterType::ADDCOL), colStat(std::move(colStat)), insAfter(insAfter) {

}

void AlterAddColStatement::print() {
    UniformLog log("AlterAddColStatement");
    std::cout << "New column: " << std::endl;
    colStat.print();
    if (insAfter) std::cout << "Insert after: " << *insAfter << std::endl;

}

void AlterAddColStatement::fillToTable(Table *table) {
    auto newCol = new ColumnDescriptor(colStat.name, colStat.defaultStr);
    table->addColumn(newCol, insAfter);
}

void ColumnStatement::print() const {
    std::cout << "Col name: " << name << ", " << (isNotNull ? "NOT NULL" : "CAN NULL");
    if (defaultStr) {
        std::cout << ", Default:" << *defaultStr;
    }
    std::cout << std::endl;
}

void AlterNothingStatement::print() {
    UniformLog log("AlterNothingColStatement");
}

AlterNothingStatement::AlterNothingStatement() : AlterStatement(AlterType::NOTHING) {

}

void AlterNothingStatement::fillToTable(Table *table) {
    // Do nothing
}

Statement *Statement::getStatement(const boost::filesystem::path &path, const boost::filesystem::path &sqlpath) {
    std::ifstream input(path.c_str());
    std::ifstream dbgInput(sqlpath.c_str());
    IOHelper dbgHelper(&dbgInput);
    {
        UniformLog log("Create Table Statement");
        std::cout << dbgHelper.getEntireFile() << std::endl;
    }
    IOHelper helper(&input);
    std::string type;
    type = helper.getLine();
    if (type == "CREATE") return CreateTableStatement::getCreateTableStatement(helper);
    else if (type == "ALTER") return AlterStatement::getAlterStatement(helper);
    else throw std::runtime_error("Unknown type of statement!");
}
