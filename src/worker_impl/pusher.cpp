#include <boost/algorithm/string.hpp>
#include "worker_impl/pusher.h"
#include "utils/iohelper.h"
#include "utils/uniformlog.h"

// Pusher should not be reliant to work descriptor after creation!

Pusher::Pusher(WorkDescriptor *workDes, Module *module) : dbName(workDes->db_name), tableName(workDes->table_name),
                                                          binlogPath(workDes->binlogPath),
                                                          stateCount(workDes->stateCount), module(module) {

}

void Pusher::push() const {
    SQLInstance *instance = module->sqlPool->getSQLInstance();
    instance->createDB(dbName);
    instance->setSchema(dbName);
    delete instance;
    createFinalTable();
    pushFromFile();
}

void Pusher::createFinalTable() const {
    SQLInstance *instance = module->sqlPool->getSQLInstance();
    instance->createDB(dbName);
    instance->setSchema(dbName);
    instance->dropTable(tableName);

    boost::filesystem::path ddlFilePath = binlogPath / std::string("finalTable.sql");
    std::ifstream ddlFile(ddlFilePath.c_str());
    std::stringstream ss;
    ss << ddlFile.rdbuf();
    std::string sqlStr = ss.str();
    UniformLog log("Pusher", dbName + "/" + tableName);
    std::cout << sqlStr << std::endl;
    try {
        instance->executeRaw(sqlStr);
    } catch (sql::SQLException &exception) {
        std::cout << "[Pusher] SQL exec failed. Detail: " << exception.what() << std::endl;
    }

    delete instance;
}

void Pusher::pushFromFile() const {
    std::ifstream headerStream(binlogPath / std::string("finalTableCols.txt"));
    std::string pushSQLHeader = std::string("INSERT INTO ") + "`" + tableName + "` (";
    std::string line;
    bool isFirst = true;
    while (std::getline(headerStream, line)) {
        if (!isFirst) pushSQLHeader += ",";
        else isFirst = false;
        pushSQLHeader += line;
    }
    pushSQLHeader += ") VALUES ";
    std::ifstream stream(binlogPath / "result.csv");
    IOHelper ioHelper(&stream);
    std::string pushSQLContent;
    int contentCount = 0;
    while (!(line = ioHelper.getLine()).empty()) {
        boost::replace_all(line, "'_NUll_&'", "null");
        if (contentCount) pushSQLContent += ",";
        pushSQLContent += "(";
        pushSQLContent += line;
        pushSQLContent += ")";
        contentCount++;
        if (contentCount >= module->config->push_split_threshold) {
            flushSQL(pushSQLHeader, pushSQLContent);
            contentCount = 0;
            pushSQLContent.clear();
        }
    }
    if (contentCount)
        flushSQL(pushSQLHeader, pushSQLContent);
}

void Pusher::flushSQL(const std::string &sqlHeader, const std::string &sqlContent) const {
    SQLInstance *instance = module->sqlPool->getSQLInstance();
    instance->setSchema(dbName);
    try {
        instance->executeRaw(sqlHeader + sqlContent);
    } catch (sql::SQLException &exception) {
        std::cout << "FAILED INSERT!!!! Because " << exception.what() << std::endl;
        std::cout << sqlHeader + sqlContent << std::endl;
    }
    delete instance;
}
