#include "statement.h"
#include "iohelper.h"
#include <fstream>
#include <iostream>

CreateTableStatement::CreateTableStatement(const boost::filesystem::path &path) {
    std::ifstream input(path.c_str());

    IOHelper helper(&input);
    std::string type, tableName;
    type = helper.getLine();

    if (type != "CREATE") throw std::runtime_error("should have create table statement!");
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
    std::cerr << "[Creat Stat] " << "Name: " << name << std::endl;
    for (const auto &col: cols) {
        std::cerr << "Col: " << col.name << (col.isNotNull ? " NOT NULL " : " CAN NULL ");
        if (col.defaultStr) std::cerr << " Def: " << *col.defaultStr;
        std::cerr << std::endl;
    }
    for (const auto &idx: indexs) {
        std::cerr << "Idx: \"" << idx.name << "\"";
        if (idx.isPrimaryKey) std::cerr << " PRI ";
        std::cerr << " Keys: ";
        for (const auto &key: idx.keys) {
            std::cerr << key << ",";
        }
        std::cerr << std::endl;
    }


}

AlterAddColStatement::AlterAddColStatement() : AlterStatement() {

}
