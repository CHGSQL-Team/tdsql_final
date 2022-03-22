#include "boost/filesystem.hpp"
#include "boost/tokenizer.hpp"
#include "structure/table.h"

class DATParser {
private:
    boost::filesystem::path datPath;
    int source;
    int &stamp;
    Table* table;
public:
    explicit DATParser(boost::filesystem::path datPath, int source, int &startStamp, Table *table);

    std::vector<Row *> parseData();
};