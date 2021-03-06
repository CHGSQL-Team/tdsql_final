#include <iostream>
#include <fstream>
#include "binfetch.h"
#include "config.h"
#include "event.h"

const unsigned char magic[] = {0xfe, 0x62, 0x69, 0x6e};


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
        std::cout <<
                  "Could not notify source server about checksum awareness."
                  "Server returned " << mysql_error(conn);
    }
    return conn;
}

void BinaryFetcher::getBinary(int source_index) {

    std::ofstream binlog( module->config->binlog_path + "/" + "binlog" +
                         std::to_string(source_index) + ".bin", std::ios::binary);
    binlog.write(reinterpret_cast<const char *>(magic), sizeof(magic));
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
            std::cout << "EOF event received. Actually received " << eventCount << " events." << std::endl;
            break;
        }
        eventReader.set(rpl.size, rpl.buffer);
        Event *event = eventReader.process();
        if (event) {
            event->_print_header();
            event->_print();
        }
        //if (event->header->type != Log_event_type::ROTATE_EVENT)
            binlog.write(reinterpret_cast<const char *>(rpl.buffer + 1), (std::streamsize) rpl.size - 1);
        eventCount++;

    }
    mysql_binlog_close(conn, &rpl);
    mysql_close(conn);
    binlog.close();
}

BinaryFetcher::BinaryFetcher(Module *module) : module(module) {
    mysql_library_init;
}

BinaryFetcher::~BinaryFetcher() {
    mysql_library_end;
}
