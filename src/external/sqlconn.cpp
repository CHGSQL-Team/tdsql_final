#include "external/sqlconn.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>


SQLInstance::SQLInstance(sql::Connection *_con, SQLPool *_pool) :
        con(_con), pool(_pool) {


}

void SQLInstance::setSchema(const std::string &name) {
    con->setSchema(name);
}

void SQLInstance::createDB(const std::string &name) {
    sql::Statement *stmt;
    stmt = con->createStatement();
    stmt->executeUpdate("CREATE DATABASE IF NOT EXISTS " + name);
    delete stmt;
}

void SQLInstance::executeRaw(const std::string &content) {
    sql::Statement *stmt;
    stmt = con->createStatement();
    stmt->executeUpdate(content);
    delete stmt;
}

sql::Connection *SQLInstance::getConnection() {
    return con;
}

SQLInstance::~SQLInstance() {
    pool->poolMutex.lock();
    pool->availConnection.push_back(con);
    pool->poolMutex.unlock();
}

//void SQLInstance::insertRows(TableDescriptor *table, std::vector<std::string> &data) {
//    if (data.empty()) return;
//    std::string sql_query = table->insert_header;
//    sql_query += "VALUES ";
//    for (const auto &row: data) {
//        sql_query += "(";
//        std::vector<std::string> values;
//        boost::split(values, row, boost::is_any_of(","));
//        for (const auto &value: values) {
//            sql_query += '\"' + value + '\"';
//            if (value != values.back()) sql_query += ",";
//        }
//        sql_query += ")";
//        if (row != data.back()) sql_query += ",";
//    }
//    sql::Statement *stmt;
//    stmt = con->createStatement();
//    stmt->executeUpdate(sql_query);
//    delete stmt;
//}

void SQLInstance::setAutoCommit(bool value) {
    con->setAutoCommit(value);
}

int SQLInstance::getRowCount(const std::string &name) {
    sql::Statement *stmt;
    stmt = con->createStatement();
    sql::ResultSet *res = stmt->executeQuery("SELECT COUNT(*) FROM `" + name + "`");
    res->next();
    int ret = res->getInt(1);
    delete res;
    delete stmt;
    return ret;
}

bool SQLInstance::isTableAvail(const std::string &db, const std::string &table) {
    return _isTableAvail_impl_show(db, table);
}

bool SQLInstance::_isTableAvail_impl_schema(const std::string &db, const std::string &table) {
    setSchema("information_schema");
    sql::Statement *stmt;
    stmt = con->createStatement();
    sql::ResultSet *res;
    if (db.empty()) {
        res = stmt->executeQuery(
                R"(SELECT db FROM PROCESSLIST WHERE (Command <> "Sleep" AND DB <> "information_schema"))");
    } else {
        res = stmt->executeQuery(
                R"(SELECT db FROM PROCESSLIST WHERE Command <> "Sleep" AND DB=")" + db +
                "\" AND INFO LIKE " + "\"%INSERT INTO `" + table + "`%\"");
    }
    res->last();
    size_t row_count = res->getRow();
    delete res;
    delete stmt;
    return row_count < 1;
}

bool SQLInstance::_isTableAvail_impl_show(const std::string &db, const std::string &table) {
    sql::Statement *stmt;
    stmt = con->createStatement();
    sql::ResultSet *res;
    res = stmt->executeQuery("SHOW PROCESSLIST");
    while (res->next()) {
        if (res->getString("Command") != "Query") continue;
        if (res->isNull("db")) continue;
        if (!db.empty() && res->getString("db") == db &&
            (res->getString("Info").find("insert into `" + db + "`.`" + table + "`") != std::string::npos ||
             res->getString("Info").find("insert into `" + table + "`") != std::string::npos)) {
            delete res;
            return false;
        }
        if (db.empty()) {
            delete res;
            return false;
        }
    }
    delete res;
    return true;
}

void SQLInstance::dropTable(const std::string &name) {
    sql::Statement *stmt;
    stmt = con->createStatement();
    stmt->executeUpdate("DROP TABLE IF EXISTS `" + name + "`");
    delete stmt;
}

void SQLInstance::commit() {
    con->commit();
}

SQLPool::SQLPool(Config *_config) {
    driver = get_driver_instance();
    config = _config;
}

SQLInstance *SQLPool::getSQLInstance() {
    SQLInstance *newInstance;
    sql::Connection *newConnection;
    poolMutex.lock();
    if (availConnection.empty()) {
        poolMutex.unlock();
        newConnection = driver->connect(config->sql_ip[2] + ":" + std::to_string(config->sql_port[2]),
                                        config->sql_usr[2], config->sql_pwd[2]);
        newInstance = new SQLInstance(newConnection, this);
        std::cout << "[SQLConn] Creating new SQL connection!" << std::endl;
    } else {
        newInstance = new SQLInstance(availConnection.front(), this);
        newInstance->getConnection()->setAutoCommit(true);
        availConnection.pop_front();
        poolMutex.unlock();
    }
    return newInstance;
}

SQLPool::~SQLPool() {
    for (auto connection: availConnection) {
        delete connection;
    }
}
