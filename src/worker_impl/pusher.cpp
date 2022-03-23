#include "worker_impl/pusher.h"
#include "utils/iohelper.h"

Pusher::Pusher(WorkDescriptor *workDes, Module *module) : workDes(workDes), module(module) {

}

void Pusher::push() {
    SQLInstance *instance = module->sqlPool->getSQLInstance();
    instance->createDB(workDes->db_name);
    instance->setSchema(workDes->db_name);
    createFinalTable();
    pushFromFile();
}

void Pusher::createFinalTable() {
    SQLInstance *instance = module->sqlPool->getSQLInstance();
    instance->createDB(workDes->db_name);
    instance->setSchema(workDes->db_name);
    instance->dropTable(workDes->table_name);
    std::vector<std::string> tableDDL;
    for (int state = 0; state < workDes->stateCount; state++) {
        boost::filesystem::path ddlFilePath = workDes->binlogPath / "0" / (std::to_string(state) + ".ddlsql");
        std::ifstream ddlFile(ddlFilePath.c_str());
        std::stringstream ss;
        ss << ddlFile.rdbuf();
        tableDDL.push_back(ss.str());
    }
    for (const auto &ddl: tableDDL) {
        instance->executeRaw(ddl);
    }
}

void Pusher::pushFromFile() {
    std::string pushSQLHeader = std::string("INSERT INTO ") + "`" + workDes->table_name + "` VALUES ";
    std::ifstream stream(workDes->binlogPath / "result.csv");
    IOHelper ioHelper(&stream);
    std::string line;
    std::string pushSQLContent;
    int contentCount = 0;
    while (!(line = ioHelper.getLine()).empty()) {
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

void Pusher::flushSQL(const std::string &sqlHeader, const std::string &sqlContent) {
    SQLInstance *instance = module->sqlPool->getSQLInstance();
    instance->setSchema(workDes->db_name);
    instance->executeRaw(sqlHeader + sqlContent);
    delete instance;
}
