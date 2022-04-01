#include "structure/module.h"
#include "mysql.h"
#include "external/jvmconn.h"

class ExternalFetcher {
protected:
    JVMConn jvmconn;
    Module *module;
public:
    explicit ExternalFetcher(Module *module);

    void dumpEventToFile(int index, const std::string &path, std::vector<unsigned char> &gtidSet);

    static MYSQL *_initConn(const char *ip, const char *usr, const char *pwd, unsigned int port);

    void evokeFetch(int start, int end, std::vector<std::vector<unsigned char>> &gtidPackages);

    std::vector<unsigned char> retrieveGtidPackage(int index);

    void evokeFetchAll(int start,int end);

};