#pragma once

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/exception.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/prepared_statement.h"
#include <string>
#include "structure/config.h"
#include "structure/descriptor.h"
#include <list>
#include <mutex>

class SQLInstance;

class SQLPool {

    Config *config;
    sql::Driver *driver;

public:
    explicit SQLPool(Config *_config);

    std::mutex poolMutex;

    SQLInstance *getSQLInstance();

    std::list<sql::Connection *> availConnection;

    ~SQLPool();
};


class SQLInstance {
    sql::Connection *con;
    std::list<SQLInstance *>::iterator it;
    SQLPool *pool;
public:
    explicit SQLInstance(sql::Connection *_con, SQLPool *_pool);

    void setSchema(const std::string &name);

    void createDB(const std::string &name);

    void dropTable(const std::string &name);

    void executeRaw(const std::string &content);

    void commit();

    sql::Connection *getConnection();

    void setAutoCommit(bool value);

    int getRowCount(const std::string &name);

    ~SQLInstance();

    bool isTableAvail(const std::string &db = "", const std::string &table = "");

    bool _isTableAvail_impl_schema(const std::string &db = "", const std::string &table = "");

    bool _isTableAvail_impl_show(const std::string &db = "", const std::string &table = "");
};