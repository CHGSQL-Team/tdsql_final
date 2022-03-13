// Using the Connector/C API to fetch binlog.

#include "mysql.h"
#include "module.h"

class BinaryFetcher {
private:
    Module* module;
    static MYSQL *_initConn(const char *ip, const char *usr, const char *pwd, unsigned int port);
public:
    BinaryFetcher(Module* module);
    void getBinary(int source_index);
    ~BinaryFetcher();


};