#include <string>
#include <boost/filesystem.hpp>
#include <map>
#include "module.h"
#include "descriptor.h"


class WorkLoader {
public:
    Module *module;

    explicit WorkLoader(Module *_module);

    void loadWorks() const;

    void _loadTable(const boost::filesystem::path &dbPath) const;

    void printWorks() const;
};