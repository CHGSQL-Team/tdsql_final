#include <iostream>
#include "binfetch.h"
#include "config.h"
#include "event.h"


void gtid_callback(MYSQL_RPL *rpl, unsigned char *packet_gtid_set) {

}


MYSQL *BinaryFetcher::_initConn(const char *ip, const char *usr, const char *pwd, unsigned int port) {
    MYSQL *conn = mysql_init(nullptr);
    if (conn == nullptr) {
        throw std::runtime_error(mysql_error(conn));
    }
    if (mysql_real_connect(conn,
                           ip,
                           usr,
                           pwd,
                           nullptr,
                           port,
                           nullptr,
                           0) == nullptr) {
        throw std::runtime_error(mysql_error(conn));
    }
    if (mysql_query(conn,
                    "SET @master_binlog_checksum = 'NONE', "
                    "@source_binlog_checksum = 'NONE'")) {
        std::cerr <<
                  "Could not notify source server about checksum awareness."
                  "Server returned " << mysql_error(conn);
    }
    return conn;
}

void BinaryFetcher::getBinary(int source_index) {

    MYSQL *conn = _initConn(module->config->sql_ip[source_index].c_str(),
                            module->config->sql_usr[source_index].c_str(),
                            module->config->sql_pwd[source_index].c_str(),
                            module->config->sql_port[source_index]);

    MYSQL_RPL rpl = {0,
                     nullptr,
                     4,
                     0,
                     MYSQL_RPL_SKIP_HEARTBEAT,
                     0,
                     nullptr,
                     nullptr,
                     0,
                     nullptr};

    if (mysql_binlog_open(conn, &rpl)) {
        throw std::runtime_error(mysql_error(conn));
    }
    EventReader eventReader;
    int eventCount = 0;
    for (;;) {
        if (mysql_binlog_fetch(conn, &rpl)) {
            throw std::runtime_error(mysql_error(conn));
        } else if (rpl.size == 0) {
            std::cerr << "EOF event received. Actually received " << eventCount << " events." << std::endl;
            break;
        }
        eventReader.set(rpl.size, rpl.buffer);
        Event* event = eventReader.process();
        if(event){
            event->_print_header();
            event->_print();
        }

        eventCount++;

    }
    mysql_binlog_close(conn, &rpl);
    mysql_close(conn);
}

BinaryFetcher::BinaryFetcher(Module *module) : module(module) {
    mysql_library_init;
}

BinaryFetcher::~BinaryFetcher() {
    mysql_library_end;
}
