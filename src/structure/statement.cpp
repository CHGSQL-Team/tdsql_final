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
        cols.emplace_back(helper);
    }
    int idxCount = helper.getLineInt();
    for (int i = 0; i < idxCount; i++) {
        std::string idxName;
        int hasName = helper.getLineInt();
        if (hasName) idxName = helper.getLine();
        int isPrimary = helper.getLineInt();
        int keyCount = helper.getLineInt();
        std::vector<std::string> keys;
        for (int j = 0; j < keyCount; j++) {
            std::string keyName = helper.getLine();
            keys.push_back(keyName);
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
        auto *newColDes = new ColumnDescriptor(col.name, col.type, col.defaultStr);
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
        return new AlterAddColStatement(helper);
    } else if (typeStr == "NOTHING") {
        return new AlterNothingStatement();
    } else if (typeStr == "DROPCOL") {
        return new AlterDropColStatement(helper);
    } else if (typeStr == "DROPINDEX") {
        return new AlterDropIndexStatement(helper);
    } else if (typeStr == "ADDINDEX") {
        return new AlterAddIndexStatement(helper);
    } else if (typeStr == "CHANGECOL") {
        return new AlterChangeColStatement(helper);
    } else throw std::runtime_error("Alter type no implemented!");
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
    auto newCol = new ColumnDescriptor(colStat.name, colStat.type, colStat.defaultStr);
    table->addColumn(newCol, insAfter);
}

AlterAddColStatement::AlterAddColStatement(IOHelper &ioHelper) : AlterStatement(AlterType::ADDCOL), colStat(ioHelper) {
    int isAfterCol = ioHelper.getLineInt();
    insAfter = nullptr;
    if (isAfterCol)
        insAfter = new std::string(ioHelper.getLine());
}

void ColumnStatement::print() const {
    std::cout << "Col name: " << name << ", " << (isNotNull ? "NOT NULL" : "CAN NULL");
    if (defaultStr) {
        std::cout << ", Default:" << *defaultStr;
    }
    std::cout << std::endl;
}

ColumnStatement::ColumnStatement(IOHelper &helper) {
    name = helper.getLine();
    type = helper.getLine();
    isNotNull = helper.getLineInt();
    if (helper.getLineInt() == 1) {
        defaultStr = new std::string(std::move(helper.getLine()));
    } else defaultStr = nullptr;
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
        UniformLog log("Statement");
        std::cout << dbgHelper.getEntireFile() << std::endl;
    }
    IOHelper helper(&input);
    std::string type;
    type = helper.getLine();
    if (type == "CREATE") return CreateTableStatement::getCreateTableStatement(helper);
    else if (type == "ALTER") return AlterStatement::getAlterStatement(helper);
    else throw std::runtime_error("Unknown type of statement! " + type);
}

AlterDropColStatement::AlterDropColStatement(std::string colName) : AlterStatement(AlterType::DROPCOL),
                                                                    colName(std::move(colName)) {

}

void AlterDropColStatement::print() {
    UniformLog log("AlterDropColStatement");
    std::cout << "Drop column: " << colName << std::endl;
}

void AlterDropColStatement::fillToTable(Table *table) {
    table->dropColumn(colName);
}

AlterDropColStatement::AlterDropColStatement(IOHelper &ioHelper) : AlterStatement(AlterType::DROPCOL) {
    colName = ioHelper.getLine();
}

void AlterDropIndexStatement::fillToTable(Table *table) {
    table->dropUniqueIndex(isPrimary, false, indexName);
}

void AlterDropIndexStatement::print() {
    UniformLog log("AlterDropIndexStatement");
    std::cout << "Drop index: " << (isPrimary ? "PRIMARY KEY" : indexName) << std::endl;
}

AlterDropIndexStatement::AlterDropIndexStatement(IOHelper &ioHelper) : AlterStatement(AlterType::DROPINDEX) {
    isPrimary = ioHelper.getLineInt();
    if (!isPrimary) {
        indexName = ioHelper.getLine();
    }
}

AlterChangeColStatement::AlterChangeColStatement(IOHelper &ioHelper) : AlterStatement(AlterType::CHANGECOL),
                                                                       colStat(ioHelper) {
    prevName = ioHelper.getLine();
}

void AlterChangeColStatement::print() {
    UniformLog log("AlterChangeColStatement");
    std::cout << "Prev col: " << prevName << ", new col:" << std::endl;
    colStat.print();
}

void AlterChangeColStatement::fillToTable(Table *table) {
    auto prevCol = std::find_if(table->colDes.begin(), table->colDes.end(), [=](const ColumnDescriptor *col) {
        return col->name == prevName;
    });
    if (prevCol == table->colDes.end()) throw std::runtime_error("Can not find column to change!");
    (*prevCol)->name = colStat.name;
    delete (*prevCol)->def;
    (*prevCol)->def = colStat.defaultStr;
}

void AlterAddIndexStatement::print() {
    UniformLog log("AlterAddIndexStatement");
    std::cout << (isPrimary ? "PRIMARY KEY" : ("Name: " + indexName)) << std::endl;
    std::cout << "Columns: ";
    for (const auto &col: cols) {
        std::cout << col << ", ";
    }
    std::cout << std::endl;
}

void AlterAddIndexStatement::fillToTable(Table *table) {

    table->addUniqueIndex(isPrimary ? "" : indexName, cols, isPrimary);
}

AlterAddIndexStatement::AlterAddIndexStatement(IOHelper &ioHelper) : AlterStatement(AlterType::ADDINDEX) {
    isPrimary = ioHelper.getLineInt();
    if (!isPrimary)
        indexName = ioHelper.getLine();
    int colCount = ioHelper.getLineInt();
    for (int i = 0; i < colCount; i++) {
        cols.push_back(ioHelper.getLine());
    }
}
