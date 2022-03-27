#include <boost/algorithm/string.hpp>
#include "worker_impl/pusher.h"
#include "utils/iohelper.h"

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
    std::vector<std::string> tableDDL;
    for (int state = 0; state < stateCount; state++) {
        boost::filesystem::path ddlFilePath = binlogPath / "0" / (std::to_string(state) + ".ddlsql");
        std::ifstream ddlFile(ddlFilePath.c_str());
        std::stringstream ss;
        ss << ddlFile.rdbuf();
        if (module->config->compress_row)
            tableDDL.push_back(ss.str() + " ROW_FORMAT = COMPRESSED KEY_BLOCK_SIZE = 32");
        else tableDDL.push_back(ss.str());
    }
    for (const auto &ddl: tableDDL) {
        instance->executeRaw(ddl);
    }
    delete instance;
}

void Pusher::pushFromFile() const {
    std::string pushSQLHeader = std::string("INSERT INTO ") + "`" + tableName + "` VALUES ";
    std::ifstream stream(binlogPath / "result.csv");
    IOHelper ioHelper(&stream);
    std::string line;
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
        std::cout << "FAILED INSERT!!!!" << std::endl;
        std::cout << sqlHeader + sqlContent << std::endl;
    }
    delete instance;
}
