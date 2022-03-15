#include "module.h"
#include "mysql.h"

class ExternalFetcher {
protected:
    Module *module;
public:
    explicit ExternalFetcher(Module *module);

    void dumpEventToFile(int index, const std::string &path, std::vector<unsigned char> &gtidSet);

    static MYSQL *_initConn(const char *ip, const char *usr, const char *pwd, unsigned int port);

    void evokeFetch(int index);

    std::vector<unsigned char> retrieveGtidPackage(int index);

};