#include "extfetcher.h"
#include "boost/filesystem.hpp"
#include <fstream>

static void fix_gtid_set(MYSQL_RPL *rpl, unsigned char *packet_gtid_set) {
    auto *gtidSet = (std::vector<unsigned char> *) rpl->gtid_set_arg;
    memcpy(packet_gtid_set, gtidSet->data(), gtidSet->size());
}

ExternalFetcher::ExternalFetcher(Module *module) : module(module), jvmconn(module) {
}

MYSQL *ExternalFetcher::_initConn(const char *ip, const char *usr, const char *pwd, unsigned int port) {
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

void ExternalFetcher::dumpEventToFile(int index, const std::string &path, std::vector<unsigned char> &gtidSet) {
    std::ofstream binlog(path, std::ios::binary);
    std::ofstream event_length(path + "len", std::ios::out);
    MYSQL *conn = _initConn(module->config->sql_ip[index].c_str(),
                            module->config->sql_usr[index].c_str(),
                            module->config->sql_pwd[index].c_str(),
                            module->config->sql_port[index]);

    MYSQL_RPL rpl = {0, nullptr, 4, 0,
                     MYSQL_RPL_SKIP_HEARTBEAT | MYSQL_RPL_GTID,
                     gtidSet.size(), fix_gtid_set, &gtidSet,
                     0, nullptr};

    if (mysql_binlog_open(conn, &rpl)) {
        throw std::runtime_error(mysql_error(conn));
    }
    int eventCount = 0;
    for (;;) {
        if (mysql_binlog_fetch(conn, &rpl)) {
            throw std::runtime_error(mysql_error(conn));
        } else if (rpl.size == 0) {
            std::cerr << "[EXTF] Received " << eventCount << " events." << std::endl;
            break;
        }
        binlog.write(reinterpret_cast<const char *>(rpl.buffer + 1), (std::streamsize) rpl.size - 1);
        event_length << rpl.size - 1 << "\n";
        eventCount++;
    }
    mysql_binlog_close(conn, &rpl);
    mysql_close(conn);
    binlog.close();
    event_length.close();
}

void ExternalFetcher::evokeFetch(int index) {
    boost::filesystem::path dumpPath(module->config->binlog_path + "/" + "binlog" + std::to_string(index) + ".bin");
    boost::filesystem::path eventLenPath(
            module->config->binlog_path + "/" + "binlog" + std::to_string(index) + ".binlen");
    std::vector<unsigned char> gtidPackage = std::move(retrieveGtidPackage(index));
    std::cerr << "[EXTF] GTIDp size = " << gtidPackage.size() << std::endl;
    dumpEventToFile(index, dumpPath.string(), gtidPackage);


    std::string callArg = boost::filesystem::canonical(dumpPath).string() + " " +
                          boost::filesystem::canonical(eventLenPath).string() + " " +
                          boost::filesystem::canonical(module->config->binlog_path).string() + " " +
                          std::to_string(index);

    jvmconn.callMethod("Entry", "canalSplit", callArg, "(Ljava/lang/String;)V", false);

    std::cerr << "[EXTF] Src " << index << " Finished" << std::endl;
    module->timed.printElapsedTime();
}

std::vector<unsigned char> ExternalFetcher::retrieveGtidPackage(int index) {
    std::string test("test");
    std::string packageString = jvmconn.callMethod("GtidResolver", "main", module->config->sql_gtid[index],
                                                   "(Ljava/lang/String;)Ljava/lang/String;", true);
    std::stringstream ss(packageString);
    std::vector<unsigned char> gtidPackage;
    unsigned int byteInt;
    while (ss >> std::hex >> byteInt) {
        gtidPackage.push_back(byteInt);
    }
    return gtidPackage;
}

void ExternalFetcher::evokeFetchAll(int start, int end) {
    for (int i = start; i <= end; i++)
        evokeFetch(i);
}
