#include "extfetcher.h"
#include "boost/filesystem.hpp"
#include <fstream>

const unsigned char magic[] = {0xfe, 0x62, 0x69, 0x6e};

std::string exec(const char *cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

static void fix_gtid_set(MYSQL_RPL *rpl, unsigned char *packet_gtid_set) {
    auto *gtidSet = (std::vector<unsigned char> *) rpl->gtid_set_arg;
    memcpy(packet_gtid_set,gtidSet->data(),gtidSet->size());
}

ExternalFetcher::ExternalFetcher(Module *module) : module(module) {

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
    binlog.write(reinterpret_cast<const char *>(magic), sizeof(magic));
    MYSQL *conn = _initConn(module->config->sql_ip[index].c_str(),
                            module->config->sql_usr[index].c_str(),
                            module->config->sql_pwd[index].c_str(),
                            module->config->sql_port[index]);

    MYSQL_RPL rpl = {0,
                     nullptr,
                     4,
                     0,
                     MYSQL_RPL_SKIP_HEARTBEAT | MYSQL_RPL_GTID,
                     gtidSet.size(),
                     fix_gtid_set,
                     &gtidSet,
                     0,
                     nullptr};

    if (mysql_binlog_open(conn, &rpl)) {
        throw std::runtime_error(mysql_error(conn));
    }
    int eventCount = 0;
    for (;;) {
        if (mysql_binlog_fetch(conn, &rpl)) {
            throw std::runtime_error(mysql_error(conn));
        } else if (rpl.size == 0) {
            std::cerr << "EOF event received. Actually received " << eventCount << " events." << std::endl;
            break;
        }
        binlog.write(reinterpret_cast<const char *>(rpl.buffer + 1), (std::streamsize) rpl.size - 1);
        eventCount++;

    }
    mysql_binlog_close(conn, &rpl);
    mysql_close(conn);
    binlog.close();
}

void ExternalFetcher::evokeFetch(int index) {
    boost::filesystem::path dumpPath(module->config->binlog_path + "/" + "binlog" +
                                     std::to_string(index) + ".bin");
    std::vector<unsigned char> gtidPackage = std::move(retrieveGtidPackage(index));
    std::cerr << "package size = " << gtidPackage.size() << std::endl;
    dumpEventToFile(index, dumpPath.string(), gtidPackage);

    std::system(
            (std::string(
                    "java -cp ../extfetcher/build/production/extfetcher:../extfetcher/libs/mysql-binlog-connector-java.jar: FileMain ")
             + boost::filesystem::absolute(dumpPath).string() + " "
             + boost::filesystem::absolute(boost::filesystem::path(module->config->binlog_path)).string()).c_str());

    std::cerr << "Dumping Finished" << std::endl;
    module->timed.printElapsedTime();
}

std::vector<unsigned char> ExternalFetcher::retrieveGtidPackage(int index) {
    std::string packageString = exec(
            (std::string(
                    "java -cp ../extfetcher/build/production/extfetcher:../extfetcher/libs/mysql-binlog-connector-java.jar: GtidResolver ")
             + module->config->sql_gtid[index]).c_str());
    std::stringstream ss(packageString);
    std::vector<unsigned char> gtidPackage;
    unsigned int byteInt;
    while (ss >> std::hex >> byteInt) {
        gtidPackage.push_back(byteInt);
    }
    return gtidPackage;

}
